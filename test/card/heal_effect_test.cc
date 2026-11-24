
#include "card/heal_effect.h"

#include <gtest/gtest.h>

#include "card/effect_context.h"
#include "player/player.h"

// Test 1: Basic healing
TEST(HealEffectTest, BasicHealing) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);

  HealEffect effect(5);
  EffectContext context{};
  context.player = &player;

  ASSERT_EQ(player.GetHp(), 20);

  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 25);
}

// Test 2: Healing cannot exceed maximum HP
TEST(HealEffectTest, CannotExceedMaxHp) {
  Player player("Knight", 30, 42);
  player.TakeDamage(2);

  HealEffect effect(10);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 30);
  EXPECT_EQ(player.GetMaxHp(), 30);
}

// Test 3: Healing does not revive dead players
TEST(HealEffectTest, CannotReviveDeadPlayer) {
  Player player("Knight", 30, 42);
  player.TakeDamage(100);

  HealEffect effect(10);
  EffectContext context{};
  context.player = &player;

  ASSERT_FALSE(player.IsAlive());

  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 0);
  EXPECT_FALSE(player.IsAlive());
}

// Test 4: Zero healing has no effect
TEST(HealEffectTest, ZeroHealingDoesNothing) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);

  HealEffect effect(0);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 20);
}

// Test 5: Negative healing has no effect
TEST(HealEffectTest, NegativeHealingDoesNothing) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);

  HealEffect effect(-5);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 20);
}
