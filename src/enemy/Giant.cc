#include "enemy/Giant.h"

Giant::Giant(int level, std::uint32_t seed)
    : Enemy(type_level(level), max_hp_level(level), dfficulty_level(level),
            level, seed) {
  giant_was_last_move_charge_ = false;
}

Enemy_skill Giant::calMove() {
  if (!is_alive()) {
    return Enemy_skill::Already_dead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (get_level()) {
    case 1:
      if (r <= 70) {
        return Enemy_skill::Giant_slam_lv1;
      } else {
        return Enemy_skill::Giant_defend_lv1;
      }
      break;
    case 2:
      if (r <= 40) {
        return Enemy_skill::Giant_slam_lv2;
      } else if (r <= 70) {
        return Enemy_skill::Giant_defend_lv2;
      } else {
        return Enemy_skill::Giant_charge_lv2;
      }
      break;
    case 3:
      if (giant_was_last_move_charge_ == true) {
        giant_was_last_move_charge_ = false;
        return Enemy_skill::Giant_super_charge_hit_lv3;
      }
      if (r <= 40) {
        return Enemy_skill::Giant_slam_lv3;
      } else if (r <= 70) {
        return Enemy_skill::Giant_defend_lv3;
      } else {
        giant_was_last_move_charge_ = true;
        return Enemy_skill::Giant_super_charge_lv3;
      }
      break;
    default:
      return Enemy_skill::Giant_slam_lv1;
      break;
  }
}

Enemy_type Giant::type_level(int level) {
  Enemy_type type;
  switch (level) {
    case 1:
      type = Enemy_type::Giant_baby;
      break;
    case 2:
      type = Enemy_type::Giant;
      break;
    case 3:
      type = Enemy_type::Giant_king;
      break;
    default:
      type = Enemy_type::Giant_baby;
      break;
  }
  return type;
}

int Giant::max_hp_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 70;
      break;
    case 2:
      temp = 95;
      break;
    case 3:
      temp = 120;
      break;
    default:
      temp = 65;
      break;
  }
  return temp;
}

int Giant::dfficulty_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 1;
      break;
    case 2:
      temp = 2;
      break;
    case 3:
      temp = 3;
      break;
    default:
      temp = 1;
      break;
  }
  return temp;
}
