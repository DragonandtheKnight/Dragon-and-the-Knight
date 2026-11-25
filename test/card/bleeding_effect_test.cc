// Planned tests (RED), disabled by default; StatusManager API is proposed.
// Assumptions: fixed split specifies HP damage;
// ratio split specifies HP fraction.
// Shield shortage, rounding, default mode and reapplication remain unspecified.
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/card.h"
#include "card/effect_context.h"
#include "card/status_manager.h"
#include "enemy/goblin.h"
#include "player/player.h"

TEST(PlannedBleeding, ExplicitShieldFirst) {
  Player s("P", 30, 42);
  Goblin t(1, 42);
  t.AddShield(3);
  StatusManager m;
  m.AddBleeding(s, t, 8, 1, BleedingMode::kShieldFirst, 0);
  EXPECT_EQ(t.GetHp(), 30);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetShield(), 0);
  EXPECT_EQ(t.GetHp(), 25);
}
TEST(PlannedBleeding, AssumptionFixedSplitWithSufficientShield) {
  Player s("P", 30, 42);
  Goblin t(1, 42);
  t.AddShield(10);
  StatusManager m;
  m.AddBleeding(s, t, 8, 1, BleedingMode::kFixedSplit, 3);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 27);
  EXPECT_EQ(t.GetShield(), 5);
}
TEST(PlannedBleeding, AssumptionRatioSplitWithEvenDivision) {
  Player s("P", 30, 42);
  Goblin t(1, 42);
  t.AddShield(10);
  StatusManager m;
  m.AddBleeding(s, t, 8, 1, BleedingMode::kRatioSplit, .25);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 28);
  EXPECT_EQ(t.GetShield(), 4);
}
