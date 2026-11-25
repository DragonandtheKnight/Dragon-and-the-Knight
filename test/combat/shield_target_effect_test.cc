#include "combat/skill_cards/shield_target_effect.h"

#include <cstdint>
#include <memory>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"

namespace {
constexpr std::uint32_t kSeed = 42;
}  // namespace

TEST(ShieldTargetEffectTest, TakesTheStoredAmountFromTheTarget) {
  Goblin goblin(1, kSeed);
  const ShieldTargetEffect effect(4);

  EffectContext context;
  context.target = &goblin;
  effect.Apply(context);

  EXPECT_EQ(goblin.GetShield(), 4);
}

TEST(ShieldTargetEffectTest, KeepsTheAmountAcrossTwoUses) {
  Goblin goblin(1, kSeed);
  const ShieldTargetEffect effect(8);

  EffectContext context;
  context.target = &goblin;
  effect.Apply(context);
  effect.Apply(context);

  EXPECT_EQ(goblin.GetShield(), 16);
}

TEST(ShieldTargetEffectTest, DoesNothingWhenTheContextHasNoTarget) {
  const ShieldTargetEffect effect(4);

  EffectContext context;
  context.target = nullptr;
  effect.Apply(context);
}

TEST(ShieldTargetEffectTest, CloneCarriesTheAmount) {
  const ShieldTargetEffect effect(8);
  const std::unique_ptr<CardEffect> copy = effect.Clone();

  Goblin goblin(1, kSeed);
  EffectContext context;
  context.target = &goblin;
  ASSERT_NE(copy, nullptr);
  copy->Apply(context);

  EXPECT_EQ(goblin.GetShield(), 8);
}

TEST(ShieldTargetEffectTest, RunsWhenTheCardCarriesIt) {
  Goblin goblin(1, kSeed);
  Card card("Dragon take flight", "Hides behind vast scales",
            CardRarity::kCommon, CardType::kDefense, 1, 5);
  card.AddEffect(std::make_unique<ShieldTargetEffect>(5));

  EffectContext context;
  context.target = &goblin;
  card.ApplyEffect(context);

  EXPECT_EQ(goblin.GetShield(), 5);
}
