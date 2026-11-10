#include "enemy/goblin.h"

#include <cstdint>

Goblin::Goblin(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {
  goblin_was_last_move_dash_ = false;
}

EnemySkill Goblin::CalculateMove() {
  if (!IsAlive()) {
    return EnemySkill::kAlreadyDead;
  }
  int r = rng_.UniformInt(1, 100);
  switch (GetLevel()) {
    case 1:
      return EnemySkill::kGoblinHitLv1;
      break;
    case 2:
      if (r <= 70) {
        return EnemySkill::kGoblinHitLv2;  // 70%概率使用技能1
      } else {
        return EnemySkill::kGoblinHardSlamLv2;
      }
      break;
    case 3:
      if (goblin_was_last_move_dash_ ==
          true) {  // 如果上一步是闪避，那么下一步闪避反击
        goblin_was_last_move_dash_ = false;
        return EnemySkill::kGoblinDashAttackLv3;
      }
      if (r <= 30) {
        return EnemySkill::kGoblinHitLv3;  // 30%概率使用技能1
      } else if (r <= 70) {
        return EnemySkill::kGoblinHardSlamLv3;
      } else {
        goblin_was_last_move_dash_ = true;
        return EnemySkill::kGoblinDashLv3;
      }
      break;
    default:
      return EnemySkill::kGoblinHitLv1;
      break;
  }
}

EnemyType Goblin::TypeForLevel(int level) {
  EnemyType type;
  switch (level) {
    case 1:
      type = EnemyType::kGoblin;
      break;
    case 2:
      type = EnemyType::kGoblinCaptain;
      break;
    case 3:
      type = EnemyType::kGoblinKing;
      break;
    default:
      type = EnemyType::kGoblin;
      break;
  }
  return type;
}

int Goblin::MaxHpForLevel(int level) {
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

int Goblin::DifficultyForLevel(int level) {
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
