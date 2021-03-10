#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>

// enum react_t { NONE, LIKE, LOVE, LOL, WOW, SAD };
using react_t = uint8_t;
constexpr react_t REACT_TYPE_MAX = 5;

// Message table
struct [[eosio::table("message"), eosio::contract("talk")]] message {
    uint64_t    id       = {}; // Non-0
    uint64_t    reply_to = {}; // Non-0 if this is a reply
    eosio::name user     = {};
    std::string content  = {};

    uint64_t primary_key() const { return id; }
    uint64_t get_reply_to() const { return reply_to; }
};

// Reaction table
struct [[eosio::table("likes"), eosio::contract("talk")]] likes {
    uint64_t    id       = {}; // Non-0
    uint64_t    reply_to = {}; // Non-0 
    eosio::name user     = {};
    react_t react        = {};  // reaction type

    uint64_t primary_key() const { return id; }
    uint64_t get_reply_to() const { return reply_to; }
};

using message_table = eosio::multi_index<
    "message"_n, message, 
    eosio::indexed_by<
        "by.reply.to"_n, 
        eosio::const_mem_fun<message, uint64_t, &message::get_reply_to>
    >
>;

using likes_table = eosio::multi_index<
    "likes"_n, likes, 
    eosio::indexed_by<
        "by.reply.to"_n, 
        eosio::const_mem_fun<likes, uint64_t, &likes::get_reply_to>
    >
>;

// The contract
class talk : eosio::contract {
  public:
    // Use contract's constructor
    using contract::contract;

    // Post a message
    [[eosio::action]] void post(uint64_t id, uint64_t reply_to, eosio::name user, const std::string& content) {
        message_table table{get_self(), 0};

        // Check user
        require_auth(user);

        // Check reply_to exists
        if (reply_to) {
            table.get(reply_to);
        }
        
        // Create an ID if user didn't specify one
        eosio::check(id < 1'000'000'000ull, "user-specified id is too big");
        if (!id)
            id = std::max(table.available_primary_key(), 1'000'000'000ull);

        // Record the message
        table.emplace(get_self(), [&](auto& message) {
            message.id       = id;
            message.reply_to = reply_to;
            message.user     = user;
            message.content  = content;
        });
    }

    // Post a message
    [[eosio::action]] void likepost(uint64_t reply_to, eosio::name user, react_t react) {
        message_table msg{get_self(), 0};
        auto likes = likes_table{get_self(), 0};

        // Check user
        require_auth(user);

        eosio::check(react <= REACT_TYPE_MAX, "Bad value for reaction");

        // if there is reaction reply_to must be non-zero
        eosio::check(reply_to > 0, "Must have valid reply-to");

        // Check reply_to exists
        auto it_msg = msg.find(reply_to);
        
        // cannot like your own post
        eosio::check(it_msg->user != user, "Cannot like yur own post");

        // determien if already liked this post
        auto idx = likes.get_index<"by.reply.to"_n>();
        auto it = idx.find(reply_to);
        if (it != idx.end()) {
            while(it != idx.end() && it->reply_to == reply_to && it->user != user) {
                it++;
            }
        }
        // if reaction is 0, post must already exist
        if(react == 0)
            eosio::check(it != idx.end() && it->user == user && it->reply_to == reply_to, "Like not found in table");

        // like does not exist, so insert new record
        if (it == idx.end() || it->reply_to != reply_to || it->user != user) {
            likes.emplace(get_self(), [&](auto& like) {
                like.id       = likes.available_primary_key();
                like.reply_to = reply_to;
                like.user     = user;
                like.react    = react;
            });
        }  
        // othwerise like already exists in talbe, so either erase it if reaction is 0 or update it
        else {
            auto like_it = likes.find(it->id);
            if (react == 0) {
                likes.erase(like_it);
            } else {
                likes.modify(like_it, get_self(), [&](auto& like) {
                    like.reply_to = reply_to;
                    like.user     = user;
                    like.react    = react;
                });
            }
        }

    }
};