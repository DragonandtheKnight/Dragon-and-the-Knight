#pragma once

#include <vector>

class Player;
class Enemy;
class Deck;
class Combat;

// The caller keeps the pointed-to objects alive during effect execution.
struct EffectContext {
  // Legacy fields used by existing cards, skills, and buffs.
  Player* player = nullptr;
  Enemy* target = nullptr;
  const std::vector<Enemy*>* enemies = nullptr;
  Deck* deck = nullptr;
  Combat* combat = nullptr;

  // Explicit targeting uses exactly one caster and the resolved recipients.
  Player* caster_player = nullptr;
  Enemy* caster_enemy = nullptr;
  Player* target_player = nullptr;
  Enemy* target_enemy = nullptr;
  std::vector<Player*> player_targets;
  std::vector<Enemy*> enemy_targets;
};
