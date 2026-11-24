
#pragma once

#include <memory>

#include "card/card_effect.h"

class BlockEffect : public CardEffect {
 public:
  explicit BlockEffect(int amount);

  void Apply(EffectContext& context) const override;

  std::unique_ptr<CardEffect> Clone() const override;

 private:
  int amount_;
};
