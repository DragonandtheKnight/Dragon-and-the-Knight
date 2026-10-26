#include "Player.h"

#include <algorithm>
#include <utility>

Player::Player(std::string name, int max_hp)
    : name_(std::move(name)),
      hp_(std::max(1, max_hp)),
      max_hp_(std::max(1, max_hp)) {}

// Block damage before HP
void Player::TakeDamage(int amount) {
  if (amount <= 0) {
    return;
  }

  int blocked = std::min(block_, amount);
  block_ -= blocked;
  amount -= blocked;

  hp_ = std::max(0, hp_ - amount);
}

// Restore HP without exceeding maximum HP
void Player::Heal(int amount) {
  if (amount <= 0) {
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

// Add gold to the player
void Player::GainGold(int amount) {
  if (amount > 0) {
    gold_ += amount;
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

// Reset energy at the beginning of a turn.
void Player::ResetEnergy() {
  energy_ = 3;
}

// Check whether the player is alive.
bool Player::IsAlive() const {
  return hp_ > 0;
}
