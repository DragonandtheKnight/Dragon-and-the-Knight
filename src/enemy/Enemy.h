#pragma once

#include <vector>

#include "enemy/Enemy_skill.h"
#include "enemy/Enemy_type.h"
#include "game/rng.h"

class Enemy {
 private:
  Enemy_type name_;
  int hp_;
  int max_hp_;
  int difficulty_;
  int shield_;
  int level_;
  bool is_alive_;

 protected:
  Rng rng_;
  std::vector<Enemy_skill> skills_;

 public:
  Enemy(Enemy_type name, int maxHp, int difficulty, int level,
        std::uint32_t seed);
  virtual ~Enemy() = default;

  Enemy_type get_name();
  void deal_dmg(int dmg);
  int get_hp();
  int get_max_hp();
  void heal(int heal);
  int get_difficulty();
  int get_level();
  void add_shield(int shield);
  void clear_shield();
  int get_shield();
  bool is_alive();
  virtual Enemy_skill calMove() = 0;
};
