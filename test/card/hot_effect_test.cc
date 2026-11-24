// Planned tests (RED), disabled by default; StatusManager API is proposed.
// Assumption: later HoT ticks occur at recipient turn start.
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/card.h"
#include "card/effect_context.h"
#include "card/status_manager.h"
#include "enemy/goblin.h"
#include "player/player.h"

TEST(PlannedHot, ThreeTotalTicksIncludeImmediateHealing) {
  Player s("S", 30, 42), t("T", 30, 43);
  t.TakeDamage(20);
  StatusManager m;
  m.AddHot(s, t, 4, 3);
  EXPECT_EQ(t.GetHp(), 14);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 18);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 22);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 22);
  EXPECT_EQ(m.Count(t), 0u);
}
TEST(PlannedHot, OneTotalTickLeavesNoFutureHealing) {
  Player s("S", 30, 42), t("T", 30, 43);
  t.TakeDamage(10);
  StatusManager m;
  m.AddHot(s, t, 4, 1);
  EXPECT_EQ(t.GetHp(), 24);
  EXPECT_EQ(m.Count(t), 0u);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 24);
}
TEST(PlannedHot, CannotReviveAndCannotExceedMaxHp) {
  Player s("S", 30, 42), dead("D", 30, 43), alive("A", 30, 44);
  dead.TakeDamage(100);
  alive.TakeDamage(2);
  StatusManager m;
  m.AddHot(s, dead, 5, 2);
  m.AddHot(s, alive, 5, 2);
  m.OnTurnStart(dead);
  m.OnTurnStart(alive);
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_FALSE(dead.IsAlive());
  EXPECT_EQ(alive.GetHp(), 30);
}
