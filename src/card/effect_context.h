#pragma once

#include <vector>

class Player;
class Enemy;
class Deck;

// All pointers are borrowed and may be null when that resource is unavailable.
// The caller keeps the objects and optional enemy list alive during execution.
// Each effect is responsible for checking the resources it needs. The list
// cannot be changed through this view, but the enemies themselves can be.
struct EffectContext {
  Player* player = nullptr;
  Enemy* target = nullptr;
  const std::vector<Enemy*>* enemies = nullptr;
  Deck* deck = nullptr;
};
