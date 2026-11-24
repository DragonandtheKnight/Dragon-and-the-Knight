// Planned tests (RED), disabled by default;
// BuffSpec/BuffManager APIs are proposed.
// Assumptions: borrowed card identity, fractional percentages,
// neutral defaults.
// Positive half ties use std::round;
// other ties/intermediate rounding remain open.
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/buff.h"
#include "card/buff_manager.h"
#include "card/card.h"
#include "card/effect_context.h"
#include "card/poison_effect.h"
#include "card/status_manager.h"
#include "enemy/goblin.h"
#include "player/player.h"
TEST(PlannedCalculation, AdditivePercentBeforeFlat) {
  Player p("P", 30, 42);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  BuffSpec a{};
  a.kind = BuffKind::kDamage;
  a.percent = .2;
  a.turns = 2;
  BuffSpec b = a;
  b.percent = .3;
  b.flat = 3;
  m.Add(p, "a", a);
  m.Add(p, "b", b);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 18);
}
TEST(PlannedCalculation, RoundAndClampAtZero) {
  Player p("P", 30, 42);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.percent = .26;
  s.turns = 2;
  m.Add(p, "round", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 6), 8);
  BuffSpec penalty{};
  penalty.kind = BuffKind::kDamage;
  penalty.flat = -100;
  penalty.turns = 2;
  m.Add(p, "penalty", penalty);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 6), 0);
}
TEST(PlannedCalculation, AssumptionPositiveHalfRoundsAwayFromZero) {
  Player p("P", 30, 42);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.percent = .5;
  s.turns = 2;
  m.Add(p, "half", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 3), 5);
}
TEST(PlannedCalculation, DamageBuffDoesNotEnhanceHealing) {
  Player p("P", 30, 42);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kHeal, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.flat = 5;
  s.turns = 2;
  m.Add(p, "damage", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kHeal, 5), 5);
}
TEST(PlannedCalculation, AllEffectOnlyChangesDamageBlockHealValues) {
  Player p("P", 30, 42);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kSpecial, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kAllEffects;
  s.percent = .2;
  s.flat = 2;
  s.turns = 2;
  m.Add(p, "all", s);
  for (auto kind : {BuffKind::kDamage, BuffKind::kBlock, BuffKind::kHeal})
    EXPECT_EQ(m.Value(p, c, kind, 10), 14);
  EXPECT_EQ(m.Value(p, c, BuffKind::kBuffMagnitude, 10), 10);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDuration, 3), 3);
  EXPECT_EQ(m.Value(p, c, BuffKind::kTriggerCount, 3), 3);
}
TEST(PlannedCalculation, InstanceDamageBuffSnapshotsDotBeforeBuffExpires) {
  // Proposal: context observers let PoisonEffect snapshot buffs on application.
  Player p("P", 30, 42);
  Goblin t(1, 42);
  BuffManager buffs;
  StatusManager statuses;
  Card card("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.percent = .2;
  s.turns = 1;
  s.card = &card;
  buffs.Add(p, "specific", s);
  EXPECT_EQ(buffs.Value(p, card, BuffKind::kDamage, 5), 6);
  EffectContext c{};
  c.caster_player = &p;
  c.target_enemy = &t;
  c.buffs = &buffs;
  c.statuses = &statuses;
  c.card = &card;
  PoisonEffect(5, 2).Apply(c);
  buffs.OnTurnEnd(p);
  EXPECT_EQ(buffs.Value(p, card, BuffKind::kDamage, 5), 5);
  statuses.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 24);
  statuses.OnTurnStart(t);
  EXPECT_EQ(t.GetHp(), 18);
}
