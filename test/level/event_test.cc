#include "level/event.h"

#include <cstdint>

#include "game/rng.h"
#include "gtest/gtest.h"
#include "level/level_type.h"
#include "player/player.h"

namespace {
constexpr uint32_t kSeed = 42;
}

TEST(EventTest, ReportsEventType) {
  Rng rng(kSeed);
  Event event(5, rng);
  EXPECT_EQ(event.GetType(), LevelType::kEvent);
}

TEST(EventTest, InvalidChoicesDoNotChangePlayer) {
  Rng rng(kSeed);
  Event event(5, rng);
  Player player("Knight", 100, kSeed);
  player.TakeDamage(20);
  event.Enter(player);
  event.ApplyChoice(player, -1);
  event.ApplyChoice(player, 99);
  EXPECT_EQ(player.GetHp(), 80);
  EXPECT_EQ(player.GetMaxHp(), 100);
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_EQ(player.GetBlock(), 0);
  EXPECT_EQ(player.GetMaxEnergy(), 10);
}

TEST(EventTest, FirstChoiceAppliesExpectedEffect) {
  Rng rng(kSeed);
  Rng prediction(kSeed);
  const int event_id = prediction.UniformInt(0, 2);
  Event event(5, rng);
  Player player("Knight", 100, kSeed);
  player.TakeDamage(20);
  event.Enter(player);
  event.ApplyChoice(player, 0);

  if (event_id == 0) {
    EXPECT_EQ(player.GetHp(), 100);
  } else if (event_id == 1) {
    EXPECT_EQ(player.GetMaxHp(), 110);
    EXPECT_EQ(player.GetHp(), 90);
  } else {
    EXPECT_EQ(player.GetGold(), 30);
  }
}

TEST(EventTest, AChoiceCannotBeAppliedTwice) {
  Rng rng(kSeed);
  Event event(5, rng);
  Player player("Knight", 100, kSeed);
  event.Enter(player);
  event.ApplyChoice(player, 0);
  const int gold = player.GetGold();
  const int hp = player.GetHp();
  const int max_hp = player.GetMaxHp();
  event.ApplyChoice(player, 0);
  EXPECT_EQ(player.GetGold(), gold);
  EXPECT_EQ(player.GetHp(), hp);
  EXPECT_EQ(player.GetMaxHp(), max_hp);
}

TEST(EventTest, SharedRngAdvancesBetweenEvents) {
  Rng rng(kSeed);
  Rng prediction(kSeed);
  Event event(5, rng);
  Player player("Knight", 100, kSeed);
  for (int i = 0; i < 3; ++i) {
    prediction.UniformInt(0, 2);
    event.Enter(player);
    event.ApplyChoice(player, -1);
  }
  EXPECT_EQ(rng.NextU32(), prediction.NextU32());
}
