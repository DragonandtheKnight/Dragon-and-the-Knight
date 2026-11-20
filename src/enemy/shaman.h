#pragma once

#include <cstdint>

#include "enemy/enemy.h"

class Shaman : public Enemy {
 public:
  Shaman(int level, std::uint32_t seed);
  EnemySkill CalculateMove() override;

 private:
  EnemyType TypeForLevel(int level);
  int MaxHpForLevel(int level);
  int DifficultyForLevel(int level);
};
