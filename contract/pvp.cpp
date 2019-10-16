#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>
#include <eosio/system.hpp>

using namespace eosio;

class [[eosio::contract("pvp")]] pvp : public eosio::contract {

  private:
    struct [[eosio::table]] st_board {
        // 仅有一条记录
        uint64_t id;
        // 登顶时间
        time_point top_time;
        // 目前积累的奖赏
        asset pool;
        // 当前头部用户
        name player_name;
        // 头部用户投注额分红后进行减半操作
        asset quantity;
        // 上次分红的时间。
        time_point last_time;

        uint64_t primary_key() const { return id; }
    };
    typedef multi_index<"board"_n, st_board> tb_board;

    // typedef eosio::multi_index<N(表名),表的对象类型> 实例化变量名
    //typedef eosio::multi_index<"boardtablea"_n, board_table> board_tables;

    struct [[eosio::table]] st_bet {
        // 桌子的标识
        uint64_t id;
        // 有效时间
        time_point valid_time;
        // 投注额
        asset quantity;
        // 桌子的哈希
        std::string table_hash;
        // 玩家1名称
        name p1_name;
        // 玩家2名称
        name p2_name;
        // 玩家1哈希
        std::string p1_hash;
        // 玩家2哈希
        std::string p2_hash;
        // 桌子的种子
        std::string table_seed;
        // 玩家1种子
        std::string p1_seed;
        // 玩家2种子
        std::string p2_seed;

        uint64_t primary_key() const { return id; }
    };
    typedef multi_index<"bet"_n, st_bet> tb_bet;
    //typedef eosio::multi_index<"bettablea"_n, bet_table> bet_tables;

    // Contract network
    const symbol network_symbol = symbol("EOS", 4);

    const name lord          = "suwzhaoihuan"_n;
    const name wallet        = "pvpbetwallet"_n;
    const int   values[16][3] = {{1000, 1960, 40},
                               {1960, 3840, 80},
                               {3840, 7520, 160},
                               {7520, 14720, 320},
                               {14720, 28800, 640},
                               {28800, 56320, 1280},
                               {56320, 110080, 2560},
                               {110080, 215040, 5120},
                               {215040, 419840, 10240},
                               {419840, 819200, 20480},
                               {819200, 1597440, 40960},
                               {1597440, 3112960, 81920},
                               {3112960, 6062080, 163840},
                               {6062080, 11796480, 327680},
                               {11796480, 22937600, 655360},
                               {22937600, 44564480, 1310720}};

    tb_bet bet_table;
    tb_board board_table;

    time_point next() { return current_time_point() + seconds(360); }

    // 投注额、赢、水
    int findValues(asset quantity) {
        for (int i = 0; i < 16; i++) {
            if (quantity.amount == values[i][0]) {
                return i;
            }
        }
        return -1;
        //check(false, "quantity index not found");
    }

    void split(const std::string& s, std::vector<std::string>& v, const std::string& c) {
        std::string::size_type pos1, pos2;
        pos2 = s.find(c);
        pos1 = 0;
        while (std::string::npos != pos2) {
            v.push_back(s.substr(pos1, pos2 - pos1));
            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if (pos1 != s.length()) {
            v.push_back(s.substr(pos1));
        }
    }

    checksum256 hex_to_sha256(const std::string& hex_str) {
        eosio::check(hex_str.length() == 64, "invalid sha256");
        checksum256 checksum;
        from_hex(hex_str, (char*)checksum.data(), sizeof(checksum.data()));
        return checksum;
    }

    uint8_t from_hex(char c) {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        eosio::check(false, "Invalid hex character");
        return 0;
    }

    size_t from_hex(const std::string& hex_str, char* out_data, size_t out_data_len) {
        auto     i       = hex_str.begin();
        uint8_t* out_pos = (uint8_t*)out_data;
        uint8_t* out_end = out_pos + out_data_len;
        while (i != hex_str.end() && out_end != out_pos) {
            *out_pos = from_hex((char)(*i)) << 4;
            ++i;
            if (i != hex_str.end()) {
                *out_pos |= from_hex((char)(*i));
                ++i;
            }
            ++out_pos;
        }
        return out_pos - (uint8_t*)out_data;
    }

  public:
    // Use contract's constructor
    using contract::contract;
    pvp(name receiver, name code, datastream<const char *> ds):contract(receiver, code, ds), eosio::symbol("EOS", 4){}

    [[eosio::on_notify("eosio.token::transfer")]]
    void bet(name from, name to, asset quantity, std::string memo) {

        if (from == lord) {
            return ;
        }
        require_auth(get_self());

        eosio::check(from != to, "cannot transfer to self");
        eosio::check(get_self() == to, "must transfer to this contract");
        eosio::check(memo.size() <= 256, "memo must smaller than 256");
        eosio::check(quantity.symbol == eosio::symbol("EOS", 4), "only accepts EOS");
        eosio::check(quantity.is_valid(), "invalid token transfer");

        int index = pvp::findValues(quantity);
        // composed by user_id-table_hash-user_hash-user_seed-player_hash
        std::vector<std::string> memos;
        pvp::split(memo, memos, "-");

        std::string user_id     = memos[0];
        std::string table_hash  = memos[1];
        std::string user_hash   = memos[2];
        std::string user_seed   = memos[3];
        std::string player_hash = memos[4];

        // 调试
        eosio::print("user_id:" + memos[0]);
        eosio::print("table_hash:" + memos[1]);
        eosio::print("user_hash:" + memos[2]);
        eosio::print("user_seed:" + memos[3]);
        eosio::print("player_hash:" + memos[4]);

        // 验证给定数据和其sha256哈希值是否匹配
        //void assert_sha256( const char* data, uint32_t length, const eosio::checksum256& hash );
        // 选手需要检查user_hash为user_seed生成。
        //const char* seed_sha256 = const_cast<char*>(user_seed.c_str());
        // 503536f15c864b6c51832a7258e294f2d25f6fd4d53e950c5f0b64073a65adb4
        // 0d471b1eb6951bd0e2f4e9eceac9594100d7f35ce67e007ce8227e411721177f
        user_seed                    = "503536f15c864b6c51832a7258e294f2d25f6fd4d53e950c5f0b64073a65adb4";
        user_hash                    = "0d471b1eb6951bd0e2f4e9eceac9594100d7f35ce67e007ce8227e411721177f";
        checksum256 user_checksum256 = hex_to_sha256(user_hash);
        const char* user_seed_str    = user_seed.c_str();
        checksum256 user_seed_hash   = eosio::sha256(user_seed_str, strlen(user_seed_str));
        // checksum256变成string
        eosio::check(user_checksum256 == user_seed_hash, "must match user hash");
        auto player_id = atoi(user_id.c_str());
        auto table_id  = player_id / 2;
        // bet_tables bettable(_code, _code.value);
        auto bet = bet_table.find(table_id);
        if (bet == bet_table.end()) {
            bet_table.emplace(get_self(), [&](auto& bet) {
                bet.id         = table_id;
                bet.valid_time = next();
                bet.quantity   = quantity;
                bet.table_hash = table_hash;
                bet.p1_name    = from;
                bet.p1_hash    = user_hash;
                bet.p2_hash    = player_hash;
                bet.p1_seed    = user_seed;
            });
        } else {
            // 选手需要检查user_hash为user_seed生成，
            // 同时检查p1_hash与player_hash相等，同时检查p2_hash与user_hash相等，
            // 同时检查table_hash与数据库的table_hash相等，同时检查数量相等。
            eosio::check(bet->valid_time >= eosio::current_time_point(), "time expired");
            eosio::check(quantity.amount == bet->quantity.amount, "must equal quantity");
            eosio::check(player_hash == bet->p1_hash, "must match player hash");
            eosio::check(user_hash == bet->p2_hash, "must match user hash");
            eosio::check(table_hash == bet->table_hash, "must match table hash");
            bet_table.modify(bet, get_self(), [&](auto& bet) {
                bet.p2_name = from;
                bet.p2_seed = user_seed;
            });
        }
    }
};