#include "card/block_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "card/effect_targets.h"
#include "card/target_type.h"
#include "enemy/enemy.h"
#include "player/player.h"

BlockEffect::BlockEffect(int amount) : amount_(amount) {}

BlockEffect::BlockEffect(int amount, TargetType target_type)
    : CardEffect(target_type, true), amount_(amount) {}

void BlockEffect::Apply(EffectContext& context) const {
  if (!UsesExplicitTargeting()) {
    if (context.player != nullptr && context.player->IsAlive()) {
      context.player->AddShield(amount_);
    }
    return;
  }

  const EffectTargets targets =
      GetLivingEffectTargets(context, GetTargetType());
  for (Player* player : targets.players) {
    player->AddShield(amount_);
  }
  for (Enemy* enemy : targets.enemies) {
    enemy->AddShield(amount_);
  }
}

std::unique_ptr<CardEffect> BlockEffect::Clone() const {
  return std::make_unique<BlockEffect>(*this);
}
