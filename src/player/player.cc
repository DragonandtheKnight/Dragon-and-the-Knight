#include "player/player.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

Player::Player(std::string name, int max_hp, uint32_t deck_seed)
    : name_(std::move(name)),
      hp_(std::max(1, max_hp)),
      max_hp_(std::max(1, max_hp)),
      deck_(deck_seed) {}

// Block damage before HP
void Player::TakeDamage(int amount) {
  if (amount <= 0 || !is_alive_) {
    return;
  }

  int blocked = std::min(block_, amount);
  block_ -= blocked;
  amount -= blocked;

  hp_ = std::max(0, hp_ - amount);

  if (hp_ == 0) {
    is_alive_ = false;
  }
}

// Restore HP without exceeding maximum HP
void Player::Heal(int amount) {
  if (amount <= 0 || !is_alive_) {
    return;
  }

  hp_ += std::min(amount, max_hp_ - hp_);
}

// Increase the player's block
void Player::AddBlock(int amount) {
  if (amount > 0) {
    block_ += amount;
  }
}

// Reset block to zero
void Player::ResetBlock() { block_ = 0; }

// Increase maximum HP and current HP
void Player::IncreaseMaxHp(int amount) {
  if (amount <= 0) {
    return;
  }

  max_hp_ += amount;

  if (is_alive_) {
    hp_ += amount;
  }
}

// Add gold to the player
void Player::GainGold(int amount) {
  if (amount > 0) {
    gold_ += amount;
    total_gold_gained_ += amount;
  }
}

// Spend gold if player has enough
bool Player::SpendGold(int amount) {
  if (amount < 0 || gold_ < amount) {
    return false;
  }

  gold_ -= amount;
  return true;
}

// Increase maximum energy
void Player::IncreaseMaxEnergy(int amount) {
  if (amount > 0) {
    max_energy_ += amount;
  }
}

// Reduce maximum energy without going below zero
void Player::ReduceMaxEnergy(int amount) {
  if (amount > 0) {
    max_energy_ = std::max(0, max_energy_ - amount);
  }
}

// Check whether the player is alive
bool Player::IsAlive() const { return is_alive_; }

// Getter
const std::string& Player::GetName() const { return name_; }

int Player::GetHp() const { return hp_; }

int Player::GetMaxHp() const { return max_hp_; }

int Player::GetBlock() const { return block_; }

int Player::GetMaxEnergy() const { return max_energy_; }

int Player::GetGold() const { return gold_; }

int Player::GetTotalGoldGained() const { return total_gold_gained_; }

// Deck access
Deck& Player::GetDeck() { return deck_; }

const Deck& Player::GetDeck() const { return deck_; }
