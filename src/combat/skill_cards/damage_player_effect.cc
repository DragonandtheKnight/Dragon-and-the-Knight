#include "combat/skill_cards/damage_player_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "player/player.h"

DamagePlayerEffect::DamagePlayerEffect(int amount) : amount_(amount) {}

void DamagePlayerEffect::Apply(EffectContext& context) const {
  if (context.player != nullptr) {
    context.player->TakeDamage(amount_);
  }
}

std::unique_ptr<CardEffect> DamagePlayerEffect::Clone() const {
  return std::make_unique<DamagePlayerEffect>(*this);
}
