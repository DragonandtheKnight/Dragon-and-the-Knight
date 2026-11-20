#include "enemy/slime.h"

#include <cstdint>

Slime::Slime(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {}

EnemySkill Slime::CalculateMove() {
  if (!IsAlive()) {
    return EnemySkill::kAlreadyDead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (GetLevel()) {
    case 1:
      if (GetHp() == GetMaxHp()) {
        return EnemySkill::kSlimeSlapLv1;
      } else if (r <= 50) {
        return EnemySkill::kSlimeSlapLv1;
      } else {
        return EnemySkill::kSlimeHealLv1;
      }
      break;
    case 2:
      if (GetHp() == GetMaxHp()) {
        return EnemySkill::kSlimeSlapLv2;
      } else if (r <= 50) {
        return EnemySkill::kSlimeSlapLv2;
      } else {
        return EnemySkill::kSlimeHealLv2;
      }
      break;
    case 3:
      if (GetHp() == GetMaxHp()) {
        return EnemySkill::kSlimeSlapLv3;
      } else if (r <= 50) {
        return EnemySkill::kSlimeSlapLv3;
      } else {
        return EnemySkill::kSlimeHealLv3;
      }
      break;
    default:
      return EnemySkill::kSlimeSlapLv1;
      break;
  }
}

EnemyType Slime::TypeForLevel(int level) {
  EnemyType type;
  switch (level) {
    case 1:
      type = EnemyType::kSlime;
      break;
    case 2:
      type = EnemyType::kSlimeHuge;
      break;
    case 3:
      type = EnemyType::kSlimeKing;
      break;
    default:
      type = EnemyType::kSlime;
      break;
  }
  return type;
}

int Slime::MaxHpForLevel(int level) {
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

int Slime::DifficultyForLevel(int level) {
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
