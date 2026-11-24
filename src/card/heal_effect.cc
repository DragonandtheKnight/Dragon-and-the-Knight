#include "card/heal_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "player/player.h"

HealEffect::HealEffect(int amount) : amount_(amount) {}

void HealEffect::Apply(EffectContext& context) const {
  if (context.player == nullptr) {
    return;
  }

  context.player->Heal(amount_);
}

std::unique_ptr<CardEffect> HealEffect::Clone() const {
  return std::make_unique<HealEffect>(amount_);
}
