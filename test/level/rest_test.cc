#include "level/rest.h"

#include <cstdint>

#include "gtest/gtest.h"
#include "level/level_type.h"
#include "player/player.h"

namespace {
constexpr uint32_t kSeed = 42;
}
TEST(RestTest, HasCorrectType) {
  Rest rest(4, 20);
  EXPECT_EQ(rest.GetId(), 4);
  EXPECT_EQ(rest.GetType(), LevelType::kRest);
}

TEST(RestTest, EnterRestoresConfiguredAmount) {
  Player player("Knight", 100, kSeed);
  Rest rest(4, 20);
  player.TakeDamage(40);
  rest.Enter(player);
  EXPECT_EQ(player.GetHp(), 80);
}

TEST(RestTest, HealingDoesNotExceedMaximum) {
  Player player("Knight", 100, kSeed);
  Rest rest(4, 50);
  player.TakeDamage(10);
  rest.HealPlayer(player);
  EXPECT_EQ(player.GetHp(), 100);
}

TEST(RestTest, CannotReviveDeadPlayer) {
  Player player("Knight", 100, kSeed);
  Rest rest(4, 50);
  player.TakeDamage(100);
  rest.Enter(player);
  EXPECT_EQ(player.GetHp(), 0);
  EXPECT_FALSE(player.IsAlive());
}

TEST(RestTest, NegativeHealingAmountIsIgnored) {
  Player player("Knight", 100, kSeed);
  Rest rest(4, -20);
  player.TakeDamage(30);
  rest.Enter(player);
  EXPECT_EQ(player.GetHp(), 70);
}
