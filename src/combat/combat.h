#pragma once

#include <vector>

#include "enemy/enemy.h"
#include "enemy/enemy_skill.h"
#include "player/player.h"

class Combat {
 public:
  static const int kDefaultApPerTurn = 3;
  static const int kMaxEnemySlots = 4;
  static const int kHandSize = 5;
  Combat(Player& player, const std::vector<Enemy*>& slots);
  void EndRound();
  int GetCurrentAp() const;
  int GetNextAp() const;
  Enemy* GetEnemy(int slot) const;
  EnemySkill GetNextAct(int slot) const;
  bool IsOver() const;
  void StartBattle();
  bool UseCard(int card_index, int target_slot);

 private:
  Player& player_;
  std::vector<Enemy*> enemies_;
  int current_ap_;
  int next_ap_;
  std::vector<EnemySkill> next_acts_;
};
