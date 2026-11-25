#pragma once

#include <memory>

#include "card/card_effect.h"

class ShieldTargetEffect : public CardEffect {
 public:
  explicit ShieldTargetEffect(int amount);
  void Apply(EffectContext& context) const override;
  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
