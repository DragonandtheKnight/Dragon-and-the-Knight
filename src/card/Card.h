#pragma once

#include <string>

#include "card/CardRarity.h"
#include "card/CardType.h"

class Player;
class Enemy;

class Card {
 private:
  std::string name;
  std::string description;
  CardRarity rarity;
  CardType type;
  int cost;
  int value;

 public:
  Card(std::string name, std::string description, CardRarity rarity,
       CardType type, int cost, int value);

  std::string getName() const;
  int getCost() const;
  CardType getType() const;
  int getValue() const;
  void applyEffect(Player& player, Enemy& enemy) const;
};
