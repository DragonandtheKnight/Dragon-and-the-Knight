#pragma once

#include <memory>

#include "card/card_effect.h"

class HealEnemiesEffect : public CardEffect {
 public:
  explicit HealEnemiesEffect(int percent);
  void Apply(EffectContext& context) const override;
  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int percent_;
};
