#pragma once

#include <memory>
#include <string>
#include <vector>

#include "card/CardRarity.h"
#include "card/CardType.h"
#include "card/card_effect.h"

class Player;
class Enemy;
struct EffectContext;

class Card {
 private:
  std::string name;
  std::string description;
  CardRarity rarity;
  CardType type;
  int cost;
  int value;
  std::vector<std::unique_ptr<CardEffect>> effects;

 public:
  Card(std::string name, std::string description, CardRarity rarity,
       CardType type, int cost, int value);
  Card(const Card& other);
  Card& operator=(const Card& other);
  Card(Card&& other) noexcept = default;
  Card& operator=(Card&& other) noexcept = default;
  ~Card() = default;

  std::string getName() const;
  std::string getDescription() const;
  CardRarity getRarity() const;
  int getCost() const;
  CardType getType() const;
  int getValue() const;

  // Takes ownership; throws std::invalid_argument for a null effect.
  void addEffect(std::unique_ptr<CardEffect> effect);
  // Executes attached effects in insertion order. Type/value remain metadata;
  // they do not create implicit effects or spend AP.
  void applyEffect(EffectContext& context) const;
  // Compatibility overload: supplies only player and target. Effects requiring
  // the enemy collection or deck must use the context overload instead.
  void applyEffect(Player& player, Enemy& enemy) const;
};
