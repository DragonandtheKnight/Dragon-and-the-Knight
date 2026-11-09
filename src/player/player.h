#pragma once

#include <cstdint>
#include <string>

#include "card/deck.h"

class Player {
 public:
  // max_hp below 1 becomes 1. deck_seed seeds the deck's own shuffle stream.
  Player(std::string name, int max_hp, uint32_t deck_seed);

  // Health and defense
  void TakeDamage(int amount);
  // Restores HP up to the maximum. Does nothing if the player is dead or
  // amount is below 1.
  void Heal(int amount);
  void AddBlock(int amount);
  // Removes all block. Battle calls it when block expires.
  void ResetBlock();
  // Raises maximum HP and current HP by amount. Amounts below 1 do nothing.
  void IncreaseMaxHp(int amount);
  bool IsAlive() const;

  // Gold
  void GainGold(int amount);
  bool SpendGold(int amount);

  // Energy. Battle tracks the current energy; Player only holds the cap, so
  // cards and buffs can raise it between battles.
  //
  // Amounts below 1 do nothing.
  void IncreaseMaxEnergy(int amount);
  void ReduceMaxEnergy(int amount);

  // Getters
  const std::string& GetName() const;
  int GetHp() const;
  int GetMaxHp() const;
  int GetBlock() const;
  int GetMaxEnergy() const;
  int GetGold() const;
  // All gold gained this run, including gold already spent.
  int GetTotalGoldGained() const;

  // Deck access
  Deck& GetDeck();
  const Deck& GetDeck() const;

 private:
  std::string name_;
  int hp_;
  int max_hp_;
  int block_ = 0;
  int max_energy_ = 10;
  int gold_ = 0;
  int total_gold_gained_ = 0;
  Deck deck_;
};
