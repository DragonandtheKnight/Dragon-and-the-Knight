
#pragma once

#include <memory>

#include "card/card_effect.h"
#include "card/target_type.h"

class HealEffect : public CardEffect {
 public:
  explicit HealEffect(int amount);
  HealEffect(int amount, TargetType target_type);

  void Apply(EffectContext& context) const override;

  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
