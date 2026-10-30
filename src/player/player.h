#pragma once

#include <string>

#include "card/Deck.h"

class Player {
 public:
  Player(std::string name, int max_hp);

  // Health and defense
  void TakeDamage(int amount);
  void Heal(int amount);
  void AddBlock(int amount);
  bool IsAlive() const;

  // Gold
  void GainGold(int amount);
  bool SpendGold(int amount);

  // Energy
  void ResetEnergy();

  // Getters
  const std::string& GetName() const;
  int GetHp() const;
  int GetMaxHp() const;
  int GetBlock() const;
  int GetEnergy() const;
  int GetGold() const;

  // Deck access
  Deck& GetDeck();
  const Deck& GetDeck() const;

 private:
  std::string name_;
  int hp_;
  int max_hp_;
  int block_ = 0;
  int energy_ = 0;
  int gold_ = 0;
  Deck deck_;
};
