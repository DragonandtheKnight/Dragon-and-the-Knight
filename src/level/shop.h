#pragma once

#include <vector>

#include "card/card.h"
#include "level/level.h"

class Shop : public Level {
 public:
  Shop(int id, std::vector<Card> cards, std::vector<int> prices);

  void Enter(Player& player) override;
  void DisplayCards() const;
  bool Buy(Player& player, int index);
  bool Sell(Player& player, int index);

  // confirmation path for a full eight card deck.
  bool ConfirmExchange(Player& player, int shop_index, int deck_index);

 private:
  bool IsValidShopIndex(int index) const;

  std::vector<Card> card_list_;
  std::vector<int> prices_;
};
