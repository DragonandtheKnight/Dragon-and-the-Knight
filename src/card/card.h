#pragma once

#include <memory>
#include <string>
#include <vector>

#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"

class Player;
class Enemy;
struct EffectContext;

class Card {
 public:
  Card(std::string name, std::string description, CardRarity rarity,
       CardType type, int cost, int value);
  Card(const Card& other);
  Card& operator=(const Card& other);
  Card(Card&& other) noexcept = default;
  Card& operator=(Card&& other) noexcept = default;
  ~Card() = default;

  std::string GetName() const;
  std::string GetDescription() const;
  CardRarity GetRarity() const;
  int GetCost() const;
  CardType GetType() const;
  int GetValue() const;

  // Takes ownership; throws std::invalid_argument for a null effect.
  void AddEffect(std::unique_ptr<CardEffect> effect);
  // Executes attached effects in insertion order. Type/value remain metadata;
  // they do not create implicit effects or spend AP.
  void ApplyEffect(EffectContext& context) const;
  // Compatibility overload: supplies only player and target. Effects requiring
  // the enemy collection or deck must use the context overload instead.
  void ApplyEffect(Player& player, Enemy& enemy) const;

 private:
  std::string name_;
  std::string description_;
  CardRarity rarity_;
  CardType type_;
  int cost_;
  int value_;
  std::vector<std::unique_ptr<CardEffect>> effects_;
};
