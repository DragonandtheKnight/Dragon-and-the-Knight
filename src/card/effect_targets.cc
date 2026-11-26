#include "card/effect_targets.h"

#include <algorithm>
#include <vector>

#include "card/effect_context.h"
#include "card/target_type.h"
#include "enemy/enemy.h"
#include "player/player.h"

namespace {

void AddLivingTarget(Player* player, std::vector<Player*>& targets) {
  if (player != nullptr && player->IsAlive() &&
      std::find(targets.begin(), targets.end(), player) == targets.end()) {
    targets.push_back(player);
  }
}

void AddLivingTarget(Enemy* enemy, std::vector<Enemy*>& targets) {
  if (enemy != nullptr && enemy->IsAlive() && !enemy->IsEmpty() &&
      std::find(targets.begin(), targets.end(), enemy) == targets.end()) {
    targets.push_back(enemy);
  }
}

}  // namespace

EffectTargets GetLivingEffectTargets(const EffectContext& context,
                                     TargetType target_type) {
  EffectTargets targets;
  const bool player_is_caster = context.caster_player != nullptr;
  const bool enemy_is_caster = context.caster_enemy != nullptr;
  if (player_is_caster == enemy_is_caster) {
    return targets;
  }
  if (enemy_is_caster && context.caster_enemy->IsEmpty()) {
    return targets;
  }

  switch (target_type) {
    case TargetType::kSelf:
      if (player_is_caster) {
        AddLivingTarget(context.caster_player, targets.players);
      } else {
        AddLivingTarget(context.caster_enemy, targets.enemies);
      }
      break;
    case TargetType::kSingleEnemy:
      if (player_is_caster) {
        AddLivingTarget(context.target_enemy, targets.enemies);
      } else {
        AddLivingTarget(context.target_player, targets.players);
      }
      break;
    case TargetType::kSingleAlly:
      if (!player_is_caster) {
        AddLivingTarget(context.target_enemy, targets.enemies);
      } else if (context.target_player == context.caster_player) {
        AddLivingTarget(context.target_player, targets.players);
      }
      break;
    case TargetType::kAllEnemies:
      if (player_is_caster) {
        for (Enemy* enemy : context.enemy_targets) {
          AddLivingTarget(enemy, targets.enemies);
        }
      } else {
        for (Player* player : context.player_targets) {
          AddLivingTarget(player, targets.players);
        }
      }
      break;
    case TargetType::kAllAllies:
      if (player_is_caster) {
        for (Player* player : context.player_targets) {
          if (player == context.caster_player) {
            AddLivingTarget(player, targets.players);
          }
        }
      } else {
        for (Enemy* enemy : context.enemy_targets) {
          AddLivingTarget(enemy, targets.enemies);
        }
      }
      break;
  }
  return targets;
}
