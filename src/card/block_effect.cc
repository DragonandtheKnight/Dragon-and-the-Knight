#include "card/block_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "player/player.h"

BlockEffect::BlockEffect(int amount) : amount_(amount) {}

void BlockEffect::Apply(EffectContext& context) const {
  if (context.player == nullptr) {
    return;
  }

  context.player->AddShield(amount_);
}

std::unique_ptr<CardEffect> BlockEffect::Clone() const {
  return std::make_unique<BlockEffect>(amount_);
}
