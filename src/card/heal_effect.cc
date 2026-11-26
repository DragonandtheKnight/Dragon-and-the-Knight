#include "card/heal_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "card/effect_targets.h"
#include "card/target_type.h"
#include "enemy/enemy.h"
#include "player/player.h"

HealEffect::HealEffect(int amount) : amount_(amount) {}

HealEffect::HealEffect(int amount, TargetType target_type)
    : CardEffect(target_type, true), amount_(amount) {}

void HealEffect::Apply(EffectContext& context) const {
  if (!UsesExplicitTargeting()) {
    if (context.player != nullptr) {
      context.player->Heal(amount_);
    }
    return;
  }

  const EffectTargets targets =
      GetLivingEffectTargets(context, GetTargetType());
  for (Player* player : targets.players) {
    player->Heal(amount_);
  }
  for (Enemy* enemy : targets.enemies) {
    enemy->Heal(amount_);
  }
}

std::unique_ptr<CardEffect> HealEffect::Clone() const {
  return std::make_unique<HealEffect>(*this);
}
