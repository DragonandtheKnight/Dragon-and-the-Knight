#include "enemy/Goblin.h"

Goblin::Goblin(int level, std::uint32_t seed)
    : Enemy(type_level(level), max_hp_level(level), dfficulty_level(level),
            level, seed) {
  goblin_was_last_move_dash_ = false;
}

Enemy_skill Goblin::calMove() {
  if (!is_alive()) {
    return Enemy_skill::Already_dead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (get_level()) {
    case 1:
      return Enemy_skill::Goblin_hit_lv1;
      break;
    case 2:
      if (r <= 70) {
        return Enemy_skill::Goblin_hit_lv2;  // 70%概率使用技能1
      } else
        return Enemy_skill::Goblin_hard_slam_lv2;
      break;
    case 3:
      if (goblin_was_last_move_dash_ ==
          true) {  // 如果上一步是闪避，那么下一步闪避反击
        goblin_was_last_move_dash_ = false;
        return Enemy_skill::Goblin_dash_attack_lv3;
      }
      if (r <= 30) {
        return Enemy_skill::Goblin_hit_lv3;  // 30%概率使用技能1
      } else if (r <= 70) {
        return Enemy_skill::Goblin_hard_slam_lv3;
      } else {
        goblin_was_last_move_dash_ = true;
        return Enemy_skill::Goblin_dash_lv3;
      }
      break;
    default:
      return Enemy_skill::Goblin_hit_lv1;
      break;
  }
}

Enemy_type Goblin::type_level(int level) {
  Enemy_type type;
  switch (level) {
    case 1:
      type = Enemy_type::Goblin;
      break;
    case 2:
      type = Enemy_type::Goblin_captain;
      break;
    case 3:
      type = Enemy_type::Goblin_king;
      break;
    default:
      type = Enemy_type::Goblin;
      break;
  }
  return type;
}

int Goblin::max_hp_level(int level) {
  int temp;
  switch (level) {
    case 1:
      temp = 30;
      break;
    case 2:
      temp = 40;
      break;
    case 3:
      temp = 50;
      break;
    default:
      temp = 30;
      break;
  }
  return temp;
}

int Goblin::dfficulty_level(int level) {
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
