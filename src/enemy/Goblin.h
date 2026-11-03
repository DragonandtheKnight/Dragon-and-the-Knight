#pragma once

#include <string>
#include <vector>

#include "enemy/Enemy.h"

class Goblin : public Enemy {
 private:
  Enemy_type goblin_type_level(int level);
  int goblin_max_hp_level(int level);
  int goblin_dfficulty_level(int level);
  bool goblin_was_last_move_dash_;

 public:
  Goblin(int level, std::uint32_t seed);
  Enemy_skill calMove() override;
};
