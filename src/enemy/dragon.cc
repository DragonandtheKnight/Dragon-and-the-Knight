#include "enemy/Dragon.h"

Dragon::Dragon(int level, std::uint32_t seed)
    : Enemy(type_level(level), max_hp_level(level), dfficulty_level(level),
            level, seed) {
  dragon_did_roar_ = false;
  dragon_was_last_move_fly_ = false;
}

Enemy_skill Dragon::calMove() {
  if (!is_alive()) {
    return Enemy_skill::Already_dead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (get_level()) {
    case 1:
      if (r <= 70) {
        return Enemy_skill::Dragon_claw_lv1;
      } else {
        return Enemy_skill::Dragon_breath_lv1;
      }
      break;
    case 2:
      if (dragon_was_last_move_fly_ == true) {
        dragon_was_last_move_fly_ = false;
        return Enemy_skill::Dragon_drop_slam_lv2;
      }
      if (get_hp() >= (get_max_hp() / 2)) {
        if (r <= 40) {
          return Enemy_skill::Dragon_claw_lv2;
        } else if (r <= 70) {
          return Enemy_skill::Dragon_breath_lv2;
        } else {
          dragon_was_last_move_fly_ = true;
          return Enemy_skill::Dragon_fly_lv2;
        }
      } else if (r <= 70) {
        return Enemy_skill::Dragon_claw_lv2;
      } else {
        return Enemy_skill::Dragon_breath_lv2;
      }
      break;
    case 3:
      if (dragon_was_last_move_fly_ == true) {
        dragon_was_last_move_fly_ = false;
        return Enemy_skill::Dragon_drop_slam_lv3;
      }
      if (get_hp() >= (get_max_hp() / 2)) {
        if (r <= 40) {
          return Enemy_skill::Dragon_claw_lv3;
        } else if (r <= 70) {
          return Enemy_skill::Dragon_breath_lv3;
        } else {
          dragon_was_last_move_fly_ = true;
          return Enemy_skill::Dragon_fly_lv3;
        }
      } else if (get_hp() >= (get_max_hp() / 4)) {
        if (r <= 20 || (r > 40 && r <= 55) || (r > 70 && r <= 85)) {
          if (dragon_did_roar_ == false) {
            dragon_did_roar_ = true;
            return Enemy_skill::Dragon_roar_lv3;
          }
        }
        if (r <= 50) {
          return Enemy_skill::Dragon_claw_lv3;
        } else {
          return Enemy_skill::Dragon_breath_lv3;
        }
      } else {
        if (dragon_did_roar_ == false) {
          dragon_did_roar_ = true;
          return Enemy_skill::Dragon_roar_lv3;
        } else {
          if (r <= 30) {
            return Enemy_skill::Dragon_claw_lv3;
          } else {
            return Enemy_skill::Dragon_breath_lv3;
          }
        }
      }
      break;
    default:
      return Enemy_skill::Dragon_claw_lv1;
      break;
  }
}

Enemy_type Dragon::type_level(int level) {
  Enemy_type type;
  switch (level) {
    case 1:
      type = Enemy_type::Dragon_baby;
      break;
    case 2:
      type = Enemy_type::Dragon_adult;
      break;
    case 3:
      type = Enemy_type::Dragon_king;
      break;
    default:
      type = Enemy_type::Dragon_baby;
      break;
  }
  return type;
}

int Dragon::max_hp_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 90;
      break;
    case 2:
      temp = 120;
      break;
    case 3:
      temp = 150;
      break;
    default:
      temp = 90;
      break;
  }
  return temp;
}

int Dragon::dfficulty_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 3;
      break;
    case 2:
      temp = 4;
      break;
    case 3:
      temp = 5;
      break;
    default:
      temp = 3;
      break;
  }
  return temp;
}
