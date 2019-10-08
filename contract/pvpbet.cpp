#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/print.hpp>

using namespace eosio;
using namespace std;

CONTRACT pvpbetwallet : public contract {
   public:
      using contract::contract;
      pvpbetwallet( name receiver, name code, datastream<const char*> ds )
         : contract(receiver, code, ds), boardtable(receiver, receiver.value), bettable(receiver, receiver.value) {}

      ACTION transfer(name from, name to, asset quantity, string memo);
      ACTION reveal(const uint64_t &table_id, const checksum256 &seed);
      ACTION send(const int index, const name &winner, const uint64_t &table_id);      

	private:
	
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
        return current_time_point() + seconds(360);
    }
  
	// 投注额、赢、水
    int findValues(asset quantity){		 
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
	
	checksum256 hex_to_sha256(const string& hex_str) {
		eosio::check(hex_str.length() == 64, "invalid sha256");
		checksum256 checksum;
		from_hex(hex_str, (char*)checksum.data(), sizeof(checksum.data()));
		return checksum;
	}

	uint8_t from_hex(char c) {
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		eosio::check(false, "Invalid hex character");
		return 0;
	}

	size_t from_hex(const string& hex_str, char* out_data, size_t out_data_len) {
		auto i = hex_str.begin();
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

 	TABLE board_table {
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
          typedef eosio::multi_index<"boardtablea"_n, board_table> board_tables;

         TABLE bet_table {
            // 桌子的标识
            uint64_t id;
            // 有效时间
            time_point valid_time;
            // 投注额
            asset quantity;
            // 桌子的哈希
            string table_hash;
            // 玩家1名称
            name p1_name;
            // 玩家2名称
            name p2_name;
            // 玩家1哈希
            string p1_hash;
            // 玩家2哈希
            string p2_hash;
            // 桌子的种子
            string table_seed;
            // 玩家1种子
            string p1_seed;
            // 玩家2种子
            string p2_seed;
      
            uint64_t primary_key() const { return id; }
          };
          typedef eosio::multi_index<"bettablea"_n, bet_table> bet_tables;
		  
      using transfer_action = action_wrapper<"transfer"_n, &pvpbetwallet::transfer>;
      using reveal_action = action_wrapper<"reveal"_n, &pvpbetwallet::reveal>;
      using send_action = action_wrapper<"send"_n, &pvpbetwallet::send>;
      
      board_tables boardtable;
	  bet_tables bettable;
};

ACTION pvpbetwallet::transfer(name from, name to, asset quantity, std::string memo){

  if (from == lord){
     // 补充能量
     return;
  }
  
  require_auth(_self);

  eosio::check(from != to, "cannot transfer to self");
  eosio::check(_self == to, "must transfer to this contract");
  eosio::check(memo.size() <= 256, "memo must smaller than 256");
  eosio::check(quantity.symbol == eosio::symbol("EOS", 4), "only accepts EOS");
  eosio::check(quantity.is_valid(), "invalid token transfer");

  int index = findValues(quantity);  
  // composed by user_id-table_hash-user_hash-user_seed-player_hash
  std::vector<std::string> memos;
  pvpbetwallet::split(memo, memos, "-");

  std::string user_id = memos[0];
  std::string table_hash = memos[1];
  std::string user_hash = memos[2];
  std::string user_seed = memos[3];
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
  user_seed = "503536f15c864b6c51832a7258e294f2d25f6fd4d53e950c5f0b64073a65adb4";
  user_hash = "0d471b1eb6951bd0e2f4e9eceac9594100d7f35ce67e007ce8227e411721177f";
  checksum256 user_checksum256 = hex_to_sha256(user_hash);
  const char* user_seed_str = user_seed.c_str();
  checksum256 user_seed_hash = eosio::sha256(user_seed_str, strlen(user_seed_str));    
  // checksum256变成string
  eosio::check(user_checksum256 == user_seed_hash, "must match user hash");
  auto player_id = atoi(user_id.c_str());
  auto table_id = player_id / 2;
  // bet_tables bettable(_code, _code.value);
  auto bet = bettable.find(table_id);
  if (bet == bettable.end()){
     bettable.emplace(_self, [&](auto &bet) {
        bet.id = table_id;
        bet.valid_time = next();
        bet.quantity = quantity;
        bet.table_hash = table_hash;
        bet.p1_name = from;
        bet.p1_hash = user_hash;
        bet.p2_hash = player_hash;
        bet.p1_seed = user_seed;
     });
  }else{
     // 选手需要检查user_hash为user_seed生成，
     // 同时检查p1_hash与player_hash相等，同时检查p2_hash与user_hash相等，
     // 同时检查table_hash与数据库的table_hash相等，同时检查数量相等。
     eosio::check(bet->valid_time >= eosio::current_time_point(), "time expired");
     eosio::check(quantity.amount == bet->quantity.amount, "must equal quantity");
     eosio::check(player_hash == bet->p1_hash, "must match player hash");
     eosio::check(user_hash == bet->p2_hash, "must match user hash");
     eosio::check(table_hash == bet->table_hash, "must match table hash");
     bettable.modify(bet, _self, [&](auto &bet) {
        bet.p2_name = from;
        bet.p2_seed = user_seed;
     });
  }
}

ACTION pvpbetwallet::reveal(const uint64_t &table_id, const eosio::checksum256 &seed){
  require_auth(_self);
  // 开奖时需要检查数据库内的table_hash为参数的seed生成。
  auto table = bettable.find(table_id);
  eosio::check(table != bettable.end(), "No table found");
  
  const char* table_seed_str = table->table_hash.c_str();
  checksum256 table_seed_hash = eosio::sha256(table_seed_str, strlen(table_seed_str));    
  // checksum256变成string
  eosio::check(seed == table_seed_hash, "must match user hash");
  
  checksum256 result = eosio::sha256((char *)&(table->table_seed), sizeof(checksum256) * 3);

  // compares first and second 4 byte chunks in result to determine a winner
  char* results = (char*)result.data();
  name winner = (results[1] < results[0]) ? table->p1_name : table->p2_name;

  auto board = boardtable.find(0);
  time_point now = eosio::current_time_point();
  if (board == boardtable.end()){
     asset zero = asset(0, symbol("EOS", 4));
     // 还没记录
     boardtable.emplace(_self, [&](auto &board) {
        board.id = 0;
        board.top_time = now;
        board.next_time = next();
        board.quantity = zero;
        board.pool = zero;
        board.player_name = lord;
     });
  }else{
     asset top = board->quantity;
     if (table->quantity >= top){
        // 替代
        boardtable.modify(board, _self, [&](auto &board) {
           board.quantity = table->quantity;
           board.player_name = winner;
           board.top_time = now;
        });
     }
  }

  int index = findValues(table->quantity);
  //  派奖
  transaction tx;
  tx.actions.emplace_back(
      permission_level{_self, "active"_n},
      _self,
      "send"_n,
      std::make_tuple(index, winner, table_id));
  tx.delay_sec = 0;
  tx.send(table_id, _self);
  // 调试
  eosio::print("table reveal:" + table->id);
  bettable.erase(table);
}

ACTION pvpbetwallet::send(const int index, const name &winner, const uint64_t &table_id){
  require_auth(_self);

  // 赢钱 from合约账号 to其他人 延时调系统transfer失败时onerror后手工处理
  action(
      permission_level{_self, "active"_n},
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(name(_self), winner, asset(values[index][1], symbol("EOS", 4)), "winner:" + table_id))
      .send();

  asset half = asset(values[index][2] / 2, symbol("EOS", 4));
  // 提成 from合约账号 to佣金账号 调系统transfer
  action(
      permission_level{_self, "active"_n},
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(name(_self), lord, half, "fee:" + table_id))
      .send();

  // 转一半到奖金池
  auto board = boardtable.find(0);
  if (now() > board->next){
     // 派彩
     asset reward = board->pool / 2;
     action(
         permission_level{_self, "active"_n},
         "eosio.token"_n,
         "transfer"_n,
         std::make_tuple(name(_self), board->player_name, reward, "reward:" + board->last))
         .send();
     // 派彩后更新时间和门槛
     boardtable.update(board, _self, [&](auto &board) {
        board.pool = board->pool - reward + half;
        board.quantity = board->quantity / 2;
        board.next = next();
     });
     // 调试
     eosio::print("send " + board->player_name + " reward " + reward);
  }else{
     boardtable.update(board, _self, [&](auto &board) {
        board.pool = board->pool + half;
     });
     // 调试
     eosio::print("board update:" + board->pool);
  }
}

extern "C" {
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {

    if (code == name("eosio.token").value && action == name("transfer").value){
      eosio::print("Action transfer\n");
      execute_action(name(receiver), name(code), &pvpbetwallet::transfer);
    }else if (code == receiver && action == name("reveal").value){
      eosio::print("Action reveal\n");
      execute_action(name(receiver), name(code), &pvpbetwallet::reveal);
    }else if (code == receiver && action == name("send").value){
      eosio::print("Action send\n");
      execute_action(name(receiver), name(code), &pvpbetwallet::send);  
    } else if( code == name("eosio").value && action == name("onerror").value ){
      eosio::print("Action onerror\n");
      execute_action(name(receiver), name(code), &pvpbetwallet::onerror);
    } else {
      eosio::print("No match action\n");
    }
  }
};
// EOSIO_DISPATCH(pvp, (transfer)(reveal)(send))
