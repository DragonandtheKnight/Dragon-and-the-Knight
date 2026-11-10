#include "enemy/dragon.h"

#include <cstdint>

Dragon::Dragon(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {
  dragon_did_roar_ = false;
  dragon_was_last_move_fly_ = false;
}

EnemySkill Dragon::CalculateMove() {
  if (!IsAlive()) {
    return EnemySkill::kAlreadyDead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (GetLevel()) {
    case 1:
      if (r <= 70) {
        return EnemySkill::kDragonClawLv1;
      } else {
        return EnemySkill::kDragonBreathLv1;
      }
      break;
    case 2:
      if (dragon_was_last_move_fly_ == true) {
        dragon_was_last_move_fly_ = false;
        return EnemySkill::kDragonDropSlamLv2;
      }
      if (GetHp() >= (GetMaxHp() / 2)) {
        if (r <= 40) {
          return EnemySkill::kDragonClawLv2;
        } else if (r <= 70) {
          return EnemySkill::kDragonBreathLv2;
        } else {
          dragon_was_last_move_fly_ = true;
          return EnemySkill::kDragonFlyLv2;
        }
      } else if (r <= 70) {
        return EnemySkill::kDragonClawLv2;
      } else {
        return EnemySkill::kDragonBreathLv2;
      }
      break;
    case 3:
      if (dragon_was_last_move_fly_ == true) {
        dragon_was_last_move_fly_ = false;
        return EnemySkill::kDragonDropSlamLv3;
      }
      if (GetHp() >= (GetMaxHp() / 2)) {
        if (r <= 40) {
          return EnemySkill::kDragonClawLv3;
        } else if (r <= 70) {
          return EnemySkill::kDragonBreathLv3;
        } else {
          dragon_was_last_move_fly_ = true;
          return EnemySkill::kDragonFlyLv3;
        }
      } else if (GetHp() >= (GetMaxHp() / 4)) {
        if (r <= 20 || (r > 40 && r <= 55) || (r > 70 && r <= 85)) {
          if (dragon_did_roar_ == false) {
            dragon_did_roar_ = true;
            return EnemySkill::kDragonRoarLv3;
          }
        }
        if (r <= 50) {
          return EnemySkill::kDragonClawLv3;
        } else {
          return EnemySkill::kDragonBreathLv3;
        }
      } else {
        if (dragon_did_roar_ == false) {
          dragon_did_roar_ = true;
          return EnemySkill::kDragonRoarLv3;
        } else {
          if (r <= 30) {
            return EnemySkill::kDragonClawLv3;
          } else {
            return EnemySkill::kDragonBreathLv3;
          }
        }
      }
      break;
    default:
      return EnemySkill::kDragonClawLv1;
      break;
  }
}

EnemyType Dragon::TypeForLevel(int level) {
  EnemyType type;
  switch (level) {
    case 1:
      type = EnemyType::kDragonBaby;
      break;
    case 2:
      type = EnemyType::kDragonAdult;
      break;
    case 3:
      type = EnemyType::kDragonKing;
      break;
    default:
      type = EnemyType::kDragonBaby;
      break;
  }
  return type;
}

int Dragon::MaxHpForLevel(int level) {
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

int Dragon::DifficultyForLevel(int level) {
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
