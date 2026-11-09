#include "enemy/Enemy.h"

#include <cstdint>

Enemy::Enemy(Enemy_type name, int max_hp, int difficulty, int level,
             std::uint32_t seed)
    : rng_(seed) {
  name_ = name;
  max_hp_ = max_hp;
  hp_ = max_hp;
  difficulty_ = difficulty;
  level_ = level;
  shield_ = 0;
  is_alive_ = true;
}

Enemy_type Enemy::get_name() { return name_; }

void Enemy::deal_dmg(int dmg) {
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

int Enemy::get_hp() { return hp_; }

int Enemy::get_max_hp() { return max_hp_; }

void Enemy::heal(int heal) {
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

int Enemy::get_difficulty() { return difficulty_; }

int Enemy::get_level() { return level_; }

void Enemy::add_shield(int shield) { shield_ = shield_ + shield; }

void Enemy::clear_shield() { shield_ = 0; }

int Enemy::get_shield() { return shield_; }

bool Enemy::is_alive() { return is_alive_; }
