#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
CONTRACT test : public contract {
public:
    using contract::contract;

    ACTION sendtoken(name from, name to){
      require_auth(from);
      //构造一笔50个EOS的资产
      auto quantity = asset(500000, symbol("EOS", 4));
      //构造一个内联action
      auto inline_action = action(
          permission_level{from, "active"_n},
          "eosio.token"_n,
          "transfer"_n,
          std::make_tuple(from, to, quantity, std::string(""))
        );
      //发送
      inline_action.send();
    }
};