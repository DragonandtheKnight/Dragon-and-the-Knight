#pragma once

#include <cstdint>

#include "enemy/enemy.h"

class Goblin : public Enemy {
 public:
  Goblin(int level, std::uint32_t seed);
  EnemySkill CalculateMove() override;

 private:
  EnemyType TypeForLevel(int level);
  int MaxHpForLevel(int level);
  int DifficultyForLevel(int level);
  bool goblin_was_last_move_dash_;
};
