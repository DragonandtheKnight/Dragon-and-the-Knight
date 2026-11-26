#pragma once

#include <memory>

#include "card/target_type.h"

struct EffectContext;

class CardEffect {
 public:
  virtual ~CardEffect() = default;

  virtual TargetType GetTargetType() const { return target_type_; }
  bool UsesExplicitTargeting() const { return uses_explicit_targeting_; }
  virtual void Apply(EffectContext& context) const = 0;
  // Return a non-null, independently owned copy of this effect and its state.
  virtual std::unique_ptr<CardEffect> Clone() const = 0;

 protected:
  CardEffect() = default;
  CardEffect(TargetType target_type, bool uses_explicit_targeting)
      : target_type_(target_type),
        uses_explicit_targeting_(uses_explicit_targeting) {}

 private:
  TargetType target_type_ = TargetType::kSelf;
  bool uses_explicit_targeting_ = false;
};
