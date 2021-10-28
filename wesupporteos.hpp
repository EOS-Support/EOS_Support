#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>
#include <eosio/system.hpp>
#include <eosio/permission.hpp> 
#include <libc/stdint.h>

using namespace eosio;

#define DONATE_TOKEN_SYMBOL symbol("EOS", 4)

#define DONATE_TOKEN_CONTRACT name("eosio.token")

// valid time to claim (90 day)
#define CLAIM_EXPIRED 7776000

namespace eden {
  uint32_t current_secs() {
    time_point tp = current_time_point();
    return tp.sec_since_epoch();
  };

  class [[eosio::contract]] wesupporteos : public contract {
  public:
    wesupporteos(name self, name first_receiver, datastream<const char*> ds) :
      contract(self, first_receiver, ds) {}

    [[eosio::on_notify("eosio.token::transfer")]]
    void on_transfer(name from, name to, asset quantity, std::string memo);

    [[eosio::action]]
    void claim(name account);
  
  private:
    struct [[eosio::table]] donate {
      uint64_t id;
      name donater;
      asset quantity;
      uint32_t donated_at;

      uint64_t primary_key() const {
        return id;
      }
    };
    typedef eosio::multi_index<name("donate"), donate> donate_tlb;
  };
}

#define EDEN_ACCOUNT name("genesisdeden")

#define EDEN_FORWARD_MEMBER(var, member)                                                    \
   decltype(auto) member()                                                                  \
   {                                                                                        \
      return std::visit([](auto& value) -> decltype(auto) { return (value.member); }, var); \
   }                                                                                        \
   decltype(auto) member() const                                                            \
   {                                                                                        \
      return std::visit([](auto& value) -> decltype(auto) { return (value.member); }, var); \
   }

#define EDEN_FORWARD_FUNCTION(var, fun)                                \
   auto fun() const                                                    \
   {                                                                   \
      return std::visit([](auto& value) { return value.fun(); }, var); \
   }

namespace edenmember {
  using member_status_type = uint8_t;
  enum member_status : member_status_type {
    pending_membership = 0,
    active_member = 1
  };

  using election_participation_status_type = uint8_t;
  enum election_participation_status : election_participation_status_type {
    not_in_election = 0,
    in_election = 1
  };

  struct member_v0 {
    eosio::name account;
    std::string name;
    member_status_type status;
    uint64_t nft_template_id;

    uint64_t primary_key() const { 
      return account.value; 
    }
  };

  struct member_v1 {
    eosio::name account;
    std::string name;
    member_status_type status;
    uint64_t nft_template_id;
    election_participation_status_type election_participation_status = not_in_election;
    uint8_t election_rank;
    eosio::name representative{uint64_t(-1)};
    std::optional<eosio::public_key> encryption_key;

    uint64_t primary_key() const { 
      return account.value; 
    }
  };
  
  using member_variant = std::variant<member_v0, member_v1>;

  struct member {
    member_variant value;
    EDEN_FORWARD_MEMBER(value, status);
    EDEN_FORWARD_FUNCTION(value, primary_key);
  };
  using member_table_type = eosio::multi_index<"member"_n, member>;

  bool is_eden(name account) {
    member_table_type member_tb(EDEN_ACCOUNT, 0);
    auto it = member_tb.find(account.value);
    if(it==member_tb.end() || !it->status()) return false;
    else return true;
  }
}