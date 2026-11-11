#pragma once

#include <cstdint>

#include "enemy/enemy.h"

class Dragon : public Enemy {
 public:
  Dragon(int level, std::uint32_t seed);
  EnemySkill CalculateMove() override;

 private:
  EnemyType TypeForLevel(int level);
  int MaxHpForLevel(int level);
  int DifficultyForLevel(int level);
  bool dragon_was_last_move_fly_;
  bool dragon_did_roar_;
};
