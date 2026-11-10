#pragma once

#include <cstdint>

#include "enemy/enemy.h"

class Slime : public Enemy {
 public:
  Slime(int level, std::uint32_t seed);
  EnemySkill CalculateMove() override;

 private:
  EnemyType TypeForLevel(int level);
  int MaxHpForLevel(int level);
  int DifficultyForLevel(int level);
};
