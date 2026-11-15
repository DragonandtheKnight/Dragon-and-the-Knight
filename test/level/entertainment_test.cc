#include "level/entertainment.h"

#include <cstdint>

#include "game/rng.h"
#include "gtest/gtest.h"
#include "level/level_type.h"
#include "player/player.h"

namespace {
constexpr uint32_t kSeed = 42;
}

TEST(EntertainmentTest, ReportsEntertainmentType) {
  Rng rng(kSeed);
  Entertainment level(6, 10, rng);
  EXPECT_EQ(level.GetType(), LevelType::kEntertainment);
}

TEST(EntertainmentTest, CannotRollWithoutPayment) {
  Rng rng(kSeed);
  Rng prediction(kSeed);
  Entertainment level(6, 10, rng);
  Player player("Knight", 100, kSeed);
  level.Enter(player);
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_EQ(rng.NextU32(), prediction.NextU32());
}

TEST(EntertainmentTest, ChargesBeforeRollingAndPaysReward) {
  Rng rng(kSeed);
  Rng prediction(kSeed);
  const int roll = prediction.UniformInt(1, 6);
  Entertainment level(6, 10, rng);
  Player player("Knight", 100, kSeed);
  player.GainGold(20);
  level.Enter(player);
  EXPECT_EQ(player.GetGold(), 10 + level.CalculateReward(roll));
}

TEST(EntertainmentTest, RewardDependsOnlyOnRoll) {
  Rng rng(kSeed);
  Entertainment level(6, 10, rng);
  EXPECT_EQ(level.CalculateReward(1), 0);
  EXPECT_EQ(level.CalculateReward(2), 0);
  EXPECT_EQ(level.CalculateReward(3), 20);
  EXPECT_EQ(level.CalculateReward(4), 20);
  EXPECT_EQ(level.CalculateReward(5), 30);
  EXPECT_EQ(level.CalculateReward(6), 50);
  EXPECT_EQ(level.CalculateReward(99), 0);
}

TEST(EntertainmentTest, RollStaysInDiceRange) {
  Rng rng(kSeed);
  Entertainment level(6, 10, rng);
  for (int i = 0; i < 100; ++i) {
    EXPECT_GE(level.Roll(), 1);
    EXPECT_LE(level.Roll(), 6);
  }
}
