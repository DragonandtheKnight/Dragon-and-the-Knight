#include "enemy/Shaman.h"

Shaman::Shaman(int level, std::uint32_t seed)
    : Enemy(type_level(level), max_hp_level(level), dfficulty_level(level),
            level, seed) {}

Enemy_skill Shaman::calMove() {
  if (!is_alive()) {
    return Enemy_skill::Already_dead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (get_level()) {
    case 1:
      if (r <= 70) {
        return Enemy_skill::Shaman_curse_lv1;
      } else {
        return Enemy_skill::Shaman_heal_all_lv1;
      }
      break;
    case 2:
      if (r <= 40) {
        return Enemy_skill::Shaman_curse_lv2;
      } else if (r <= 70) {
        return Enemy_skill::Shaman_heal_all_lv2;
      } else {
        return Enemy_skill::Shaman_give_shield_lv2;
      }
      break;
    case 3:
      if (get_hp() <= (get_max_hp() / 2)) {
        return Enemy_skill::Shaman_heal_self_lv3;
      }
      if (r <= 20) {
        return Enemy_skill::Shaman_curse_lv3;
      } else if (r <= 70) {
        return Enemy_skill::Shaman_heal_all_lv3;
      } else {
        return Enemy_skill::Shaman_give_shield_lv3;
      }
      break;
    default:
      return Enemy_skill::Shaman_curse_lv1;
      break;
  }
}

Enemy_type Shaman::type_level(int level) {
  Enemy_type type;
  switch (level) {
    case 1:
      type = Enemy_type::Shaman;
      break;
    case 2:
      type = Enemy_type::Shaman_leader;
      break;
    case 3:
      type = Enemy_type::shaman_king;
      break;
    default:
      type = Enemy_type::Shaman;
      break;
  }
  return type;
}

int Shaman::max_hp_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 40;
      break;
    case 2:
      temp = 55;
      break;
    case 3:
      temp = 70;
      break;
    default:
      temp = 40;
      break;
  }
  return temp;
}

int Shaman::dfficulty_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 2;
      break;
    case 2:
      temp = 3;
      break;
    case 3:
      temp = 4;
      break;
    default:
      temp = 2;
      break;
  }
  return temp;
}
