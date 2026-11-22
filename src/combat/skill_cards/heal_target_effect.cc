#include "combat/skill_cards/heal_target_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "enemy/enemy.h"
#include "player/player.h"

HealTargetEffect::HealTargetEffect(int amount) : amount_(amount) {}

void HealTargetEffect::Apply(EffectContext& context) const {
  if (context.target != nullptr) {
    context.target->Heal(amount_);
  }
}

std::unique_ptr<CardEffect> HealTargetEffect::Clone() const {
  return std::make_unique<HealTargetEffect>(*this);
}
