#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace eosio;

class [[eosio::contract("hodl")]] hodl : public eosio::contract {
  private:
    static const uint32_t the_party = 1645525342;
    const symbol hodl_symbol;

    struct [[eosio::table]] balance
    {
      eosio::asset funds;
      uint64_t primary_key() const { return funds.symbol.raw(); }
    };

    using balance_table = eosio::multi_index<"balance"_n, balance>;

    uint32_t now() {
      return current_time_point().sec_since_epoch();
    }

  public:
    using contract::contract;

    hodl(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),hodl_symbol("SYS", 4){}

    [[eosio::on_notify("eosio.token::transfer")]]
    void deposit(name hodler, name to, eosio::asset quantity, std::string memo) {
      if (hodler == get_self() || to != get_self())
      {
        return;
      }

      check(now() < the_party, "You're way late");
      check(quantity.amount > 0, "When pigs fly");
      check(quantity.symbol == hodl_symbol, "These are not the droids you are looking for.");

      balance_table balance(get_self(), hodler.value);
      auto hodl_it = balance.find(hodl_symbol.raw());

      if (hodl_it != balance.end())
        balance.modify(hodl_it, get_self(), [&](auto &row) {
          row.funds += quantity;
        });
      else
        balance.emplace(get_self(), [&](auto &row) {
          row.funds = quantity;
        });
    }

    [[eosio::action]]
    void party(name hodler)
    {
      //Check the authority of hodlder
      require_auth(hodler);

      // //Check the current time has pass the the party time
      check(now() > the_party, "Hold your horses");

      balance_table balance(get_self(), hodler.value);
      auto hodl_it = balance.find(hodl_symbol.raw());

      // //Make sure the holder is in the table
      check(hodl_it != balance.end(), "You're not allowed to party");

      action{
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(get_self(), hodler, hodl_it->funds, std::string("Party! Your hodl is free."))
      }.send();

      balance.erase(hodl_it);
    }
};