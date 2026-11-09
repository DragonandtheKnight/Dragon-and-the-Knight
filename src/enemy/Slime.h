#pragma once

#include <string>
#include <vector>

#include "enemy/Enemy.h"

class Slime : public Enemy {
 private:
  Enemy_type type_level(int level);
  int max_hp_level(int level);
  int dfficulty_level(int level);

 public:
  Slime(int level, std::uint32_t seed);
  Enemy_skill calMove() override;
};
