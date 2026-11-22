#include "combat/skill_cards/heal_target_effect.h"

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

TEST(HealTargetEffectTest, TakesTheStoredAmountFromTheTarget) {
  Goblin goblin(1, kSeed);
  goblin.TakeDamage(9);
  const HealTargetEffect effect(4);

  EffectContext context;
  context.target = &goblin;
  effect.Apply(context);

  EXPECT_EQ(goblin.GetHp(), 25);
}

TEST(HealTargetEffectTest, KeepsTheAmountAcrossTwoUses) {
  Goblin goblin(1, kSeed);
  goblin.TakeDamage(20);
  const HealTargetEffect effect(8);

  EffectContext context;
  context.target = &goblin;
  effect.Apply(context);
  effect.Apply(context);

  EXPECT_EQ(goblin.GetHp(), 26);
}

TEST(HealTargetEffectTest, DoesNothingWhenTheContextHasNoTarget) {
  const HealTargetEffect effect(4);

  EffectContext context;
  context.target = nullptr;
  effect.Apply(context);
}

TEST(HealTargetEffectTest, CloneCarriesTheAmount) {
  const HealTargetEffect effect(8);
  const std::unique_ptr<CardEffect> copy = effect.Clone();

  Goblin goblin(1, kSeed);
  goblin.TakeDamage(20);
  EffectContext context;
  context.target = &goblin;
  ASSERT_NE(copy, nullptr);
  copy->Apply(context);

  EXPECT_EQ(goblin.GetHp(), 18);
}

TEST(HealTargetEffectTest, RunsWhenTheCardCarriesIt) {
  Goblin goblin(1, kSeed);
  goblin.TakeDamage(9);
  Card card("Slime mend", "Mends the caster", CardRarity::kCommon,
            CardType::kHeal, 1, 5);
  card.AddEffect(std::make_unique<HealTargetEffect>(5));

  EffectContext context;
  context.target = &goblin;
  card.ApplyEffect(context);

  EXPECT_EQ(goblin.GetHp(), 26);
}
