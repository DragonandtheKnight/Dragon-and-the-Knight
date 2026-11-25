
#include "card/damage_effect.h"

#include <gtest/gtest.h>

#include <memory>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "enemy/goblin.h"

// Test 1: Basic damage without shield
TEST(DamageEffectTest, BasicDamage) {
  Goblin goblin(1, 42);
  DamageEffect effect(8);

  EffectContext context{};
  context.target = &goblin;

  ASSERT_EQ(goblin.GetHp(), 30);

  effect.Apply(context);

  EXPECT_EQ(goblin.GetHp(), 22);
}

// Test 2: Shield absorbs part of the damage
TEST(DamageEffectTest, ShieldAbsorbsDamage) {
  Goblin goblin(1, 42);
  goblin.AddShield(3);

  DamageEffect effect(8);
  EffectContext context{};
  context.target = &goblin;

  effect.Apply(context);

  EXPECT_EQ(goblin.GetShield(), 0);
  EXPECT_EQ(goblin.GetHp(), 25);
}

// Test 3: Shield absorbs all damage
TEST(DamageEffectTest, FullShieldProtection) {
  Goblin goblin(1, 42);
  goblin.AddShield(10);

  DamageEffect effect(8);
  EffectContext context{};
  context.target = &goblin;

  effect.Apply(context);

  EXPECT_EQ(goblin.GetShield(), 2);
  EXPECT_EQ(goblin.GetHp(), 30);
}

// Test 4: Fatal damage kills the enemy
TEST(DamageEffectTest, FatalDamage) {
  Goblin goblin(1, 42);
  DamageEffect effect(40);

  EffectContext context{};
  context.target = &goblin;

  effect.Apply(context);

  EXPECT_EQ(goblin.GetHp(), 0);
  EXPECT_FALSE(goblin.IsAlive());
}

// Test 5: A cloned effect preserves its behavior
TEST(DamageEffectTest, ClonePreservesDamage) {
  Goblin goblin(1, 42);
  DamageEffect effect(8);

  std::unique_ptr<CardEffect> clone = effect.Clone();
  ASSERT_NE(clone, nullptr);

  EffectContext context{};
  context.target = &goblin;

  clone->Apply(context);

  EXPECT_EQ(goblin.GetHp(), 22);
}

// Test 6: A Card can contain a DamageEffect
TEST(DamageEffectTest, SlashCardAppliesDamage) {
  Goblin goblin(1, 42);

  Card slash("Slash", "Deal 8 damage", CardRarity::kCommon, CardType::kAttack,
             1, 8);

  slash.AddEffect(std::make_unique<DamageEffect>(8));

  EffectContext context{};
  context.target = &goblin;

  slash.ApplyEffect(context);

  EXPECT_EQ(goblin.GetHp(), 22);
  EXPECT_EQ(slash.GetCost(), 1);
}
