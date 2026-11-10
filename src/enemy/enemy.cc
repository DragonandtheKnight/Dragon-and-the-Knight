#include "enemy/enemy.h"

#include <cstdint>

Enemy::Enemy(EnemyType name, int max_hp, int difficulty, int level,
             std::uint32_t seed)
    : rng_(seed),
      name_(name),
      hp_(max_hp),
      max_hp_(max_hp),
      difficulty_(difficulty),
      shield_(0),
      level_(level),
      is_alive_(true) {}

EnemyType Enemy::GetName() const { return name_; }

void Enemy::TakeDamage(int dmg) {
  if (dmg <= 0) return;
  int temp;
  temp = hp_ + shield_ - dmg;
  if (temp <= 0) {
    hp_ = 0;
    shield_ = 0;
    is_alive_ = false;
    return;
  }
  if (dmg <= shield_) {
    shield_ = shield_ - dmg;
    return;
  } else {
    dmg = dmg - shield_;
    shield_ = 0;
    hp_ = hp_ - dmg;
    return;
  }
}

int Enemy::GetHp() const { return hp_; }

int Enemy::GetMaxHp() const { return max_hp_; }

void Enemy::Heal(int heal) {
  if (heal <= 0) return;
  int temp;
  temp = heal + hp_;
  if (!is_alive_) {
    return;
  }
  if (temp < max_hp_) {
    hp_ = temp;
    return;
  } else {
    hp_ = max_hp_;
  }
}

int Enemy::GetDifficulty() const { return difficulty_; }

int Enemy::GetLevel() const { return level_; }

void Enemy::AddShield(int shield) {
  if (shield <= 0) return;
  shield_ = shield_ + shield;
}

void Enemy::ClearShield() { shield_ = 0; }

int Enemy::GetShield() const { return shield_; }

bool Enemy::IsAlive() const { return is_alive_; }
