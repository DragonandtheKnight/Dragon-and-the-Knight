#pragma once

#include <cstdint>
#include <vector>

#include "enemy/enemy_skill.h"
#include "enemy/enemy_type.h"
#include "game/rng.h"

class Enemy {
 public:
  Enemy(EnemyType name, int max_hp, int difficulty, int level,
        std::uint32_t seed);
  virtual ~Enemy() = default;

  EnemyType GetName() const;
  void TakeDamage(int dmg);
  int GetHp() const;
  int GetMaxHp() const;
  void Heal(int heal);
  int GetDifficulty() const;
  int GetLevel() const;
  void AddShield(int shield);
  void ClearShield();
  int GetShield() const;
  bool IsAlive() const;
  virtual EnemySkill CalculateMove() = 0;

 protected:
  Rng rng_;
  std::vector<EnemySkill> skills_;

 private:
  EnemyType name_;
  int hp_;
  int max_hp_;
  int difficulty_;
  int shield_;
  int level_;
  bool is_alive_;
};
