#pragma once

#include <cstdint>

#include "enemy/enemy.h"

class Giant : public Enemy {
 public:
  Giant(int level, std::uint32_t seed);
  EnemySkill CalculateMove() override;

 private:
  EnemyType TypeForLevel(int level);
  int MaxHpForLevel(int level);
  int DifficultyForLevel(int level);
  bool giant_was_last_move_charge_;
};
