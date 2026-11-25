#include "enemy/empty_enemy.h"

#include <cstdint>

EmptyEnemy::EmptyEnemy(int level, std::uint32_t seed)
    : Enemy(TypeForLevel(level), MaxHpForLevel(level),
            DifficultyForLevel(level), level, seed) {}

EnemySkill EmptyEnemy::CalculateMove() { return EnemySkill::kAlreadyDead; }

EnemyType EmptyEnemy::TypeForLevel(int level) {
  EnemyType type;
  type = EnemyType::kEmpty;
  return type;
}

int EmptyEnemy::MaxHpForLevel(int level) { return 0; }

int EmptyEnemy::DifficultyForLevel(int level) { return 0; }
