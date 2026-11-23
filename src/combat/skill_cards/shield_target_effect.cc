#include "combat/skill_cards/shield_target_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "enemy/enemy.h"
#include "player/player.h"

ShieldTargetEffect::ShieldTargetEffect(int amount) : amount_(amount) {}

void ShieldTargetEffect::Apply(EffectContext& context) const {
  if (context.target != nullptr) {
    context.target->AddShield(amount_);
  }
}

std::unique_ptr<CardEffect> ShieldTargetEffect::Clone() const {
  return std::make_unique<ShieldTargetEffect>(*this);
}
