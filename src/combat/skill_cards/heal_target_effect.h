#pragma once

#include <memory>

#include "card/card_effect.h"

class HealTargetEffect : public CardEffect {
 public:
  explicit HealTargetEffect(int amount);
  void Apply(EffectContext& context) const override;
  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
