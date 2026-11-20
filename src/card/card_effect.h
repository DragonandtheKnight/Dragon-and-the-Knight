#pragma once

#include <memory>

struct EffectContext;

class CardEffect {
 public:
  virtual ~CardEffect() = default;

  virtual void Apply(EffectContext& context) const = 0;
  // Return a non-null, independently owned copy of this effect and its state.
  virtual std::unique_ptr<CardEffect> Clone() const = 0;
};
