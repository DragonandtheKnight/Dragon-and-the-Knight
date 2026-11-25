#pragma once

#include <memory>

#include "card/card_effect.h"

class DamagePlayerEffect : public CardEffect {
 public:
  explicit DamagePlayerEffect(int amount);
  void Apply(EffectContext& context) const override;
  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
