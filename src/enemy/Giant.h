#pragma once

#include <string>
#include <vector>

#include "enemy/Enemy.h"

class Giant : public Enemy {
 private:
  Enemy_type type_level(int level);
  int max_hp_level(int level);
  int dfficulty_level(int level);
  bool giant_was_last_move_charge_;

 public:
  Giant(int level, std::uint32_t seed);
  Enemy_skill calMove() override;
};
