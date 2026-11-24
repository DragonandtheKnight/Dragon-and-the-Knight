#include "card/damage_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "enemy/enemy.h"

DamageEffect::DamageEffect(int amount) : amount_(amount) {}

void DamageEffect::Apply(EffectContext& context) const {
  if (context.target == nullptr) {
    return;
  }

  context.target->TakeDamage(amount_);
}

std::unique_ptr<CardEffect> DamageEffect::Clone() const {
  return std::make_unique<DamageEffect>(amount_);
}
