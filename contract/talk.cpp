#include <eosio/eosio.hpp>

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

uint128_t makeLikesUniqSecondary( uint64_t post_id, eosio::name user) {
    return uint128_t{post_id} << 64 | user.value;
}

// Likes table
struct [[eosio::table("likes"), eosio::contract("talk")]] likes {
    uint64_t    id       = {}; // Non-0
    uint64_t    post_id  = {}; // Non-0 
    eosio::name user     = {};
    react_t react        = {};  // reaction type

    uint64_t primary_key() const { return id; }
    uint128_t get_uniq_secondary() const { return makeLikesUniqSecondary(post_id, user); }
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
        "by.uniq.secondary"_n, 
        eosio::const_mem_fun<likes, uint128_t, &likes::get_uniq_secondary>
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

    // Like or unlike a message, or change reaction
    // to like a message, react must be > 0 and <+ REACT_MAX
    // to unlike a message, react must be 0 and like must exist in likes_table
    [[eosio::action]] void likepost(uint64_t post_id, eosio::name user, react_t react) {
        // Check user authentication
        require_auth(user);

        auto msg = message_table{get_self(), 0};
        auto likes = likes_table{get_self(), 0};

        // check for valid reaction 
        eosio::check(react <= REACT_TYPE_MAX, "Bad value for reaction");

        // post_id must be > 0
        eosio::check(post_id > 0, "Must have valid post_id-to");

        // Check reply_to exists
        auto it_msg = msg.find(post_id);
        
        // cannot like your own post
        eosio::check(it_msg->user != user, "Cannot like yur own post");

        // determien if user already liked this post
        auto idx = likes.get_index<"by.uniq.secondary"_n>();
        auto it = idx.find(makeLikesUniqSecondary(post_id, user));
        bool likeExists = (it != idx.end());
        
        // if reaction is 0, post must already exist (it will be deleted)
        if(react == 0)
            eosio::check(likeExists, "Like not found in table");

        // like does not exist, so insert new record
        if (!likeExists) {
            likes.emplace(get_self(), [&](auto& like) {
                like.id       = likes.available_primary_key();
                like.post_id  = post_id;
                like.user     = user;
                like.react    = react;
            });
        }  
        // othwerise like already exists in table, so either erase it, if reaction is 0 or update it it reaction is changed
        else {
            auto like_it = likes.find(it->id);
            if (react == 0) {
                likes.erase(like_it);
            } else if (react != like_it->react) {
                likes.modify(like_it, get_self(), [&](auto& like) {
                    like.react    = react;
                });
            }
        }

    }
};
