#pragma once

#include <string>

#include "card/Deck.h"

class Player {
 private:
  std::string name;
  int hp;
  int maxHp;
  int block = 0;
  int energy = 0;
  int gold = 0;
  Deck deck;

 public:
  Player(std::string name, int maxHp);

  void takeDamage(int amount);
  void heal(int amount);
  void addBlock(int amount);
  void gainGold(int amount);
  bool spendGold(int amount);
  void resetEnergy();
  bool isAlive() const;
};
