#include "enemy/shaman.h"

#include <cstdint>

Shaman::Shaman(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {}

EnemySkill Shaman::CalculateMove() {
  if (!IsAlive()) {
    return EnemySkill::kAlreadyDead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (GetLevel()) {
    case 1:
      if (r <= 70) {
        return EnemySkill::kShamanCurseLv1;
      } else {
        return EnemySkill::kShamanHealAllLv1;
      }
      break;
    case 2:
      if (r <= 40) {
        return EnemySkill::kShamanCurseLv2;
      } else if (r <= 70) {
        return EnemySkill::kShamanHealAllLv2;
      } else {
        return EnemySkill::kShamanGiveShieldLv2;
      }
      break;
    case 3:
      if (GetHp() <= (GetMaxHp() / 2)) {
        return EnemySkill::kShamanHealSelfLv3;
      }
      if (r <= 20) {
        return EnemySkill::kShamanCurseLv3;
      } else if (r <= 70) {
        return EnemySkill::kShamanHealAllLv3;
      } else {
        return EnemySkill::kShamanGiveShieldLv3;
      }
      break;
    default:
      return EnemySkill::kShamanCurseLv1;
      break;
  }
}

EnemyType Shaman::TypeForLevel(int level) {
  EnemyType type;
  switch (level) {
    case 1:
      type = EnemyType::kShaman;
      break;
    case 2:
      type = EnemyType::kShamanLeader;
      break;
    case 3:
      type = EnemyType::kShamanKing;
      break;
    default:
      type = EnemyType::kShaman;
      break;
  }
  return type;
}

int Shaman::MaxHpForLevel(int level) {
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

int Shaman::DifficultyForLevel(int level) {
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
