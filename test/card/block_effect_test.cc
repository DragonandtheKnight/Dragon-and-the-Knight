
#include "card/block_effect.h"

#include <gtest/gtest.h>

#include "card/effect_context.h"
#include "player/player.h"

// Test 1: Basic shield generation
TEST(BlockEffectTest, BasicBlocking) {
  Player player("Knight", 30, 42);

  BlockEffect effect(6);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetShield(), 6);
}

// Test 2: Shield can stack
TEST(BlockEffectTest, ShieldStacks) {
  Player player("Knight", 30, 42);
  player.AddShield(4);

  BlockEffect effect(6);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetShield(), 10);
}

// Test 3: Shield absorbs incoming damage
TEST(BlockEffectTest, ShieldProtectsPlayer) {
  Player player("Knight", 30, 42);

  BlockEffect effect(6);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);
  player.TakeDamage(4);

  EXPECT_EQ(player.GetShield(), 2);
  EXPECT_EQ(player.GetHp(), 30);
}

// Test 4: Zero block adds no shield
TEST(BlockEffectTest, ZeroBlockDoesNothing) {
  Player player("Knight", 30, 42);

  BlockEffect effect(0);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetShield(), 0);
}

// Test 5: Negative block adds no shield
TEST(BlockEffectTest, NegativeBlockDoesNothing) {
  Player player("Knight", 30, 42);

  BlockEffect effect(-5);
  EffectContext context{};
  context.player = &player;

  effect.Apply(context);

  EXPECT_EQ(player.GetShield(), 0);
}
