#pragma once

#include <vector>

#include "card/card.h"
#include "combat/buff.h"
#include "enemy/enemy.h"
#include "enemy/enemy_skill.h"
#include "player/player.h"

class Combat {
 public:
  static constexpr int kDefaultApPerTurn = 3;
  static constexpr int kMaxEnemySlots = 4;
  static constexpr int kHandSize = 5;
  Combat(Player& player, const std::vector<Enemy*>& slots);
  void EndRound();
  int GetCurrentAp() const;
  int GetNextAp() const;
  void AddNextAp(int amount);
  Enemy* GetEnemy(int slot) const;
  EnemySkill GetNextAct(int slot) const;
  bool IsOver() const;
  void StartBattle();
  bool UseCard(int card_index, int target_slot);
  Buff& GetBuff();
  const Buff& GetBuff() const;

 private:
  Buff buff_;
  Player& player_;
  std::vector<Enemy*> enemies_;
  int current_ap_;
  int next_ap_;
  std::vector<EnemySkill> next_acts_;
  virtual Card* GetSkillCard(EnemySkill skill) const;
  void ResolveOneSkill(EnemySkill skill, int slot);
  void ResolveEnemySkills();
};
