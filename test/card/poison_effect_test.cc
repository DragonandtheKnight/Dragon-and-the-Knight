// Planned tests (RED), disabled by default; StatusManager API is proposed.
// AddPoison receives snapshot damage;
// effect/buff integration is tested separately.
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/card.h"
#include "card/effect_context.h"
#include "card/status_manager.h"
#include "enemy/goblin.h"
#include "player/player.h"

TEST(PlannedPoison, NoImmediateTickThenTargetTurnsIgnoreShieldAndExpire) {
  Player source("P", 30, 42);
  Goblin target(1, 42);
  target.AddShield(10);
  StatusManager m;
  m.AddPoison(source, target, 5, 2);
  EXPECT_EQ(target.GetHp(), 30);
  m.OnTurnStart(source);
  EXPECT_EQ(target.GetHp(), 30);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 25);
  EXPECT_EQ(target.GetShield(), 10);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 20);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 20);
  EXPECT_EQ(m.Count(target), 0u);
}
TEST(PlannedPoison, ReapplicationsKeepIndependentSnapshotsAndTickCounts) {
  Player source("P", 30, 42);
  Goblin target(1, 42);
  StatusManager m;
  m.AddPoison(source, target, 3, 2);
  m.OnTurnStart(target);
  m.AddPoison(source, target, 7, 1);
  EXPECT_EQ(m.Count(target), 2u);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 17);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 17);
  EXPECT_EQ(m.Count(target), 0u);
}
TEST(PlannedPoison, EnemyPoisonsPlayer) {
  Goblin source(1, 42);
  Player target("P", 30, 42);
  StatusManager m;
  target.AddShield(8);
  m.AddPoison(source, target, 4, 1);
  m.OnTurnStart(target);
  EXPECT_EQ(target.GetHp(), 26);
  EXPECT_EQ(target.GetShield(), 8);
}
