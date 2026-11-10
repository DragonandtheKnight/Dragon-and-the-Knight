#include "enemy/Slime.h"

Slime::Slime(int level, std::uint32_t seed)
    : Enemy(type_level(level), max_hp_level(level), dfficulty_level(level),
            level, seed) {}

Enemy_skill Slime::calMove() {
  if (!is_alive()) {
    return Enemy_skill::Already_dead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (get_level()) {
    case 1:
      if (get_hp() == get_max_hp()) {
        return Enemy_skill::Slime_slap_lv1;
      } else if (r <= 50) {
        return Enemy_skill::Slime_slap_lv1;
      } else {
        return Enemy_skill::Slime_heal_lv1;
      }
      break;
    case 2:
      if (get_hp() == get_max_hp()) {
        return Enemy_skill::Slime_slap_lv2;
      } else if (r <= 50) {
        return Enemy_skill::Slime_slap_lv2;
      } else {
        return Enemy_skill::Slime_heal_lv2;
      }
      break;
    case 3:
      if (get_hp() == get_max_hp()) {
        return Enemy_skill::Slime_slap_lv3;
      } else if (r <= 50) {
        return Enemy_skill::Slime_slap_lv3;
      } else {
        return Enemy_skill::Slime_heal_lv3;
      }
      break;
    default:
      return Enemy_skill::Slime_slap_lv1;
      break;
  }
}

Enemy_type Slime::type_level(int level) {
  Enemy_type type;
  switch (level) {
    case 1:
      type = Enemy_type::Slime;
      break;
    case 2:
      type = Enemy_type::Slime_huge;
      break;
    case 3:
      type = Enemy_type::Slime_king;
      break;
    default:
      type = Enemy_type::Slime;
      break;
  }
  return type;
}

int Slime::max_hp_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 20;
      break;
    case 2:
      temp = 30;
      break;
    case 3:
      temp = 40;
      break;
    default:
      temp = 20;
      break;
  }
  return temp;
}

int Slime::dfficulty_level(int level) {
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
