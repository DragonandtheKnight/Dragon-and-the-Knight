#pragma once

#include <string>
#include <vector>

#include "enemy/Enemy.h"

class Dragon : public Enemy {
 private:
  Enemy_type type_level(int level);
  int max_hp_level(int level);
  int dfficulty_level(int level);
  bool dragon_was_last_move_fly_;
  bool dragon_did_roar_;

 public:
  Dragon(int level, std::uint32_t seed);
  Enemy_skill calMove() override;
};
