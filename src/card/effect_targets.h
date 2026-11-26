#pragma once

#include <vector>

#include "card/target_type.h"

class Enemy;
class Player;
struct EffectContext;

struct EffectTargets {
  std::vector<Player*> players;
  std::vector<Enemy*> enemies;
};

// Reads the supplied targets for an effect and skips invalid recipients.
EffectTargets GetLivingEffectTargets(const EffectContext& context,
                                     TargetType target_type);
