
#pragma once

#include <memory>

#include "card/card_effect.h"

class HealEffect : public CardEffect {
 public:
  explicit HealEffect(int amount);

  void Apply(EffectContext& context) const override;

  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
