// #include <eosiolib/eosio.hpp>
// #include <eosiolib/time.hpp>
// #include <eosiolib/asset.hpp>
// #include <eosiolib/print.hpp>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/print.hpp>

namespace pvpbetwallet{

    using namespace eosio;
    using namespace std;

    // EOSIO network
      const string symbol_name = "EOS";
      // Contract network
      const symbol network_symbol = symbol(symbol_name, 4);

      const name & lord   = name("suwzhaoihuan");
      const name & wallet = name("pvpbetwallet");
    
      const int values[16][3] = {
        {1000, 1960, 40},
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
        {22937600, 44564480, 1310720}
      };

  time_point next(){
    return current_time_point() + 360000;
  }
  
  int findIndex(asset quantity){
    for (int i = 0; i < 16; i++){
      if (quantity.amount == values[i][0]){
          return i;
      }
    }
    check(false, "quantity index not found");
  }

  void split(const std::string &s, vector<std::string> &v, const std::string &c){
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2){
      v.push_back(s.substr(pos1, pos2 - pos1));
      pos1 = pos2 + c.size();
      pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length()){
      v.push_back(s.substr(pos1));
    }
  }

  uint8_t from_hex(char c){
    if (c >= '0' && c <= '9')
      return c - '0';
    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;
    check(false, "Invalid hex character");
    return 0;
  }

  size_t from_hex(const std::string &hex_str, char *out_data, size_t out_data_len){
    auto i = hex_str.begin();
    uint8_t *out_pos = (uint8_t *)out_data;
    uint8_t *out_end = out_pos + out_data_len;
    while (i != hex_str.end() && out_end != out_pos){
      *out_pos = from_hex((char)(*i)) << 4;
      ++i;
      if (i != hex_str.end()){
        *out_pos |= from_hex((char)(*i));
        ++i;
      }
      ++out_pos;
    }
    return out_pos - (uint8_t *)out_data;
  }

  checksum256 hex_to_sha256(const std::string &hex_str){
    check(hex_str.length() == 64, "invalid sha256");
    checksum256 checksum;
    from_hex(hex_str, (char *)checksum.hash, sizeof(checksum.hash));
    return checksum;
  }

  std::string to_hex(const char *d, uint32_t s){
    std::string r;
    const char *to_hex = "0123456789abcdef";
    uint8_t *c = (uint8_t *)d;
    for (uint32_t i = 0; i < s; ++i)
      (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
    return r;
  }

  std::string sha256_to_hex(const checksum256 &sha256){
    return to_hex((char *)sha256.hash, sizeof(sha256.hash));
  }

  void assert_sha256(const std::string &seed, const checksum256 &hash){
    assert_sha256(seed.c_str(), strlen(seed.c_str()), (const eosio::checksum256 *)&hash);
  }

  void assert_sha256(const checksum256 &seed, const checksum256 &hash){
    std::string seed_str = sha256_to_hex(seed);
    assert_sha256(seed_str, hash);
  }

  void assert_sha256(const std::string &seed, const std::string &hash){
    checksum256 hash_value = hex_to_sha256(hash);
    assert_sha256(seed, hash_value);
  }
  
  class[[eosio::contract("pvpbetwallet")]] pvpbetwallet : public contract::contract{
    
    public:
      pvpbetwallet(name receiver, name code, datastream<const char*> ds)::contract(receiver, code, ds) {}
      
      [[eosio::action]] void transfer(name from, name to, asset quantity, string memo);
  
      [[eosio::action]] void reveal(const uint64_t table_id, const checksum256 seed);
  
      [[eosio::action]] void send(const int index, const name winner, const uint64_t table_id);                                  
      
    private:      
      
      struct [[eosio::table]] boardtable {
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
        // 进行下次分红的时间。
        time_point next_time;
        
        uint64_t primary_key() const { return id; }
      };
      // typedef eosio::multi_index<N(表名),表的对象类型> 实例化变量名
      eosio::multi_index<name("boardtable"), boardtable> boards;
  
      struct [[eosio::table]] bet_table {
        // 桌子的标识
        uint64_t id;
        // 有效时间
        time_point valid_time;
        // 投注额
        asset quantity;
        // 桌子的哈希
        checksum256 table_hash;
        // 玩家1名称
        name p1_name;
        // 玩家2名称
        name p2_name;
        // 玩家1哈希
        checksum256 p1_hash;
        // 玩家2哈希
        checksum256 p2_hash;
        // 桌子的种子
        checksum256 table_seed;
        // 玩家1种子
        checksum256 p1_seed;
        // 玩家2种子
        checksum256 p2_seed;
  
        uint64_t primary_key() const { return id; }
      };
      eosio::multi_index<name("bettable"), bettable> bets;
  };
}// namespace pvpbet
