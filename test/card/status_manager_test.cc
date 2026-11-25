// Planned tests (RED), disabled by default; StatusManager API is proposed.
// Battle reports deaths; OnTurnStart returns whether the owner may act.
#include "card/status_manager.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/card.h"
#include "card/effect_context.h"
#include "card/poison_effect.h"
#include "enemy/goblin.h"
#include "player/player.h"
TEST(PlannedStatus, SourceDeathRemovesOnlyItsDotsAcrossTargets) {
  Player a("A", 30, 42), b("B", 30, 43);
  Goblin x(1, 42), y(1, 43);
  StatusManager m;
  m.AddPoison(a, x, 3, 2);
  m.AddPoison(b, x, 5, 2);
  m.AddBleeding(a, y, 4, 2, BleedingMode::kShieldFirst, 0);
  a.TakeDamage(100);
  m.OnDeath(a);
  EXPECT_EQ(m.Count(x), 1u);
  EXPECT_EQ(m.Count(y), 0u);
  m.OnTurnStart(x);
  EXPECT_EQ(x.GetHp(), 25);
}
TEST(PlannedStatus, TargetDeathClearsAllStatuses) {
  Player s("S", 30, 42);
  Goblin t(1, 42);
  StatusManager m;
  m.AddPoison(s, t, 2, 3);
  m.AddHot(s, t, 2, 3);
  t.TakeDamage(100);
  m.OnDeath(t);
  EXPECT_EQ(m.Count(t), 0u);
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 0);
}
TEST(PlannedStatus, FifoLethalPoisonStopsLaterBleedAndPreventsAction) {
  Player s("S", 30, 42);
  Goblin t(1, 42);
  t.AddShield(10);
  StatusManager m;
  m.AddPoison(s, t, 30, 1);
  m.AddBleeding(s, t, 5, 1, BleedingMode::kShieldFirst, 0);
  EXPECT_FALSE(m.OnTurnStart(t));
  EXPECT_EQ(t.GetHp(), 0);
  EXPECT_EQ(t.GetShield(), 10);
  EXPECT_EQ(m.Count(t), 0u);
}
TEST(PlannedStatus, EarlierBleedExecutesBeforeLethalPoison) {
  Player s("S", 30, 42);
  Goblin t(1, 42);
  t.AddShield(10);
  StatusManager m;
  m.AddBleeding(s, t, 5, 1, BleedingMode::kShieldFirst, 0);
  m.AddPoison(s, t, 30, 1);
  EXPECT_FALSE(m.OnTurnStart(t));
  EXPECT_EQ(t.GetShield(), 5);
}
TEST(PlannedStatus, AppliedStatusOutlivesEffect) {
  // Proposal: context.statuses and PoisonEffect(base, count).
  Player s("S", 30, 42);
  Goblin t(1, 42);
  StatusManager m;
  EffectContext c{};
  c.caster_player = &s;
  c.target_enemy = &t;
  c.statuses = &m;
  {
    PoisonEffect effect(4, 2);
    effect.Apply(c);
  }
  m.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 26);
}
