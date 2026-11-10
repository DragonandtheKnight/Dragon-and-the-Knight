#include "enemy/giant.h"

#include <cstdint>

Giant::Giant(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {
  giant_was_last_move_charge_ = false;
}

EnemySkill Giant::CalculateMove() {
  if (!IsAlive()) {
    return EnemySkill::kAlreadyDead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (GetLevel()) {
    case 1:
      if (r <= 70) {
        return EnemySkill::kGiantSlamLv1;
      } else {
        return EnemySkill::kGiantDefendLv1;
      }
      break;
    case 2:
      if (r <= 40) {
        return EnemySkill::kGiantSlamLv2;
      } else if (r <= 70) {
        return EnemySkill::kGiantDefendLv2;
      } else {
        return EnemySkill::kGiantChargeLv2;
      }
      break;
    case 3:
      if (giant_was_last_move_charge_ == true) {
        giant_was_last_move_charge_ = false;
        return EnemySkill::kGiantSuperChargeHitLv3;
      }
      if (r <= 40) {
        return EnemySkill::kGiantSlamLv3;
      } else if (r <= 70) {
        return EnemySkill::kGiantDefendLv3;
      } else {
        giant_was_last_move_charge_ = true;
        return EnemySkill::kGiantSuperChargeLv3;
      }
      break;
    default:
      return EnemySkill::kGiantSlamLv1;
      break;
  }
}

EnemyType Giant::TypeForLevel(int level) {
  EnemyType type;
  switch (level) {
    case 1:
      type = EnemyType::kGiantBaby;
      break;
    case 2:
      type = EnemyType::kGiant;
      break;
    case 3:
      type = EnemyType::kGiantKing;
      break;
    default:
      type = EnemyType::kGiantBaby;
      break;
  }
  return type;
}

int Giant::MaxHpForLevel(int level) {
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

int Giant::DifficultyForLevel(int level) {
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
