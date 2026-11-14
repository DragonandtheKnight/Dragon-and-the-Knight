#include "level/shop.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "level/level_type.h"
#include "player/player.h"

namespace {
constexpr std::size_t kDeckLimit = 8;
constexpr int kSellReward = 5;
}  // namespace

Shop::Shop(int id, std::vector<Card> cards, std::vector<int> prices)
    : Level(id, LevelType::kShop, false),
      card_list_(std::move(cards)),
      prices_(std::move(prices)) {
  if (card_list_.size() != prices_.size()) {
    throw std::invalid_argument("Shop cards and prices must match");
  }
  for (int price : prices_) {
    if (price < 0) {
      throw std::invalid_argument("Shop prices cannot be negative");
    }
  }
}

void Shop::Enter(Player& player) {
  (void)player;
  DisplayCards();
  // UI selects a card and explicitly calls Buy or ConfirmExchange.
}

void Shop::DisplayCards() const {
  for (std::size_t i = 0; i < card_list_.size(); ++i) {
    std::cout << i << ": " << card_list_[i].GetName() << " - " << prices_[i]
              << " gold\n";
  }
}

bool Shop::IsValidShopIndex(int index) const {
  return index >= 0 && static_cast<std::size_t>(index) < card_list_.size();
}

bool Shop::Buy(Player& player, int index) {
  if (!IsValidShopIndex(index) ||
      player.GetDeck().GetCards().size() >= kDeckLimit ||
      player.GetGold() < prices_[index]) {
    return false;
  }

  Card selected = card_list_[index];
  if (!player.SpendGold(prices_[index])) {
    return false;
  }
  player.GetDeck().AddCard(selected);
  return true;
}

bool Shop::ConfirmExchange(Player& player, int shop_index, int deck_index) {
  if (!IsValidShopIndex(shop_index) || deck_index < 0 ||
      player.GetDeck().GetCards().size() != kDeckLimit ||
      static_cast<std::size_t>(deck_index) >= kDeckLimit ||
      player.GetGold() < prices_[shop_index]) {
    return false;
  }

  // no gold is spent until the caller confirms the exact card to replace.
  Card selected = card_list_[shop_index];
  if (!player.SpendGold(prices_[shop_index])) {
    return false;
  }
  player.GetDeck().RemoveCard(deck_index);
  player.GetDeck().AddCard(selected);
  return true;
}

bool Shop::Sell(Player& player, int index) {
  // do not break a required eight card deck.
  const auto& cards = player.GetDeck().GetCards();
  if (index < 0 || cards.size() <= kDeckLimit ||
      static_cast<std::size_t>(index) >= cards.size()) {
    return false;
  }
  if (!player.GetDeck().RemoveCard(index)) {
    return false;
  }
  player.GainGold(kSellReward);
  return true;
}
