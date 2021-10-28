#include "wesupporteos.hpp"

namespace eden {
  void wesupporteos::on_transfer(name from, name to, asset quantity, std::string memo) {
    if (from == _self || to != _self) return;

    if (quantity.symbol != DONATE_TOKEN_SYMBOL || 
      get_first_receiver() != DONATE_TOKEN_CONTRACT) return;

    name memember = name(memo);
    if (!is_account(memember)) return;

    check(edenmember::is_eden(memember), "the donation object must be eden member");

    donate_tlb donates(_self, memember.value);
    uint64_t id = donates.available_primary_key();
    if (id == 0) id =1;

    donates.emplace(_self, [&](auto& row) {
      row.id = id;
      row.donater = from;
      row.quantity = quantity;
      row.donated_at = current_secs();
    });
  };

  void wesupporteos::claim(name account) {
    require_auth(account);

    uint32_t now = current_secs();

    asset unclaimed = asset(0, DONATE_TOKEN_SYMBOL);

    donate_tlb donates(_self, account.value);
    auto itr = donates.begin();
    while (itr != donates.end()) {
      if (now - itr->donated_at <= CLAIM_EXPIRED) {
        unclaimed += itr->quantity;
      }
      itr = donates.erase(itr);
    }

    action(
      permission_level{_self, name("active")},
      DONATE_TOKEN_CONTRACT,
      name("transfer"),
      std::make_tuple(_self, account, unclaimed, std::string("claim donation"))
    ).send();
  };
}