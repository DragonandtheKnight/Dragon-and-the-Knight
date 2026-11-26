#include "card/damage_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "card/effect_targets.h"
#include "card/target_type.h"
#include "enemy/enemy.h"
#include "player/player.h"

DamageEffect::DamageEffect(int amount)
    : CardEffect(TargetType::kSingleEnemy, false), amount_(amount) {}

DamageEffect::DamageEffect(int amount, TargetType target_type)
    : CardEffect(target_type, true), amount_(amount) {}

void DamageEffect::Apply(EffectContext& context) const {
  if (!UsesExplicitTargeting()) {
    if (context.target != nullptr && context.target->IsAlive() &&
        !context.target->IsEmpty()) {
      context.target->TakeDamage(amount_);
    }
    return;
  }

  const EffectTargets targets =
      GetLivingEffectTargets(context, GetTargetType());
  for (Player* player : targets.players) {
    player->TakeDamage(amount_);
  }
  for (Enemy* enemy : targets.enemies) {
    enemy->TakeDamage(amount_);
  }
}

std::unique_ptr<CardEffect> DamageEffect::Clone() const {
  return std::make_unique<DamageEffect>(*this);
}
