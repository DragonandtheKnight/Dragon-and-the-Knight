#pragma once

#include <memory>

#include "card/card_effect.h"

class ShieldMaxHpEnemyEffect : public CardEffect {
 public:
  explicit ShieldMaxHpEnemyEffect(int amount);
  void Apply(EffectContext& context) const override;
  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
