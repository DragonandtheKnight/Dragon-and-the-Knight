// Planned tests (RED), disabled by default;
// BuffSpec/BuffManager APIs are proposed.
// Assumptions: borrowed card identity, fractional percentages,
// neutral defaults.
// Unequal-duration refresh and identity transfer remain unspecified.
#include "card/buff.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/buff_manager.h"
#include "card/card.h"
#include "card/effect_context.h"
#include "enemy/goblin.h"
#include "player/player.h"

TEST(PlannedBuff, ImmediateAndCurrentOwnerTurnCounts) {
  Player p("P", 30, 42), other("O", 30, 43);
  BuffManager m;
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.flat = 3;
  s.turns = 1;
  m.Add(p, "a", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 13);
  m.OnTurnEnd(other);
  EXPECT_EQ(m.RemainingTurns(p, "a"), 1);
  m.OnTurnEnd(p);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 10);
}
TEST(PlannedBuff, ExplicitExtendAndRefreshDurations) {
  Player p("P", 30, 42);
  BuffManager m;
  BuffSpec s{};
  s.kind = BuffKind::kDamage;
  s.flat = 2;
  s.turns = 2;
  s.stacking = StackingRule::kExtend;
  m.Add(p, "extend", s);
  m.Add(p, "extend", s);
  EXPECT_EQ(m.RemainingTurns(p, "extend"), 4);
  s.stacking = StackingRule::kRefresh;
  m.Add(p, "refresh", s);
  m.Add(p, "refresh", s);
  EXPECT_EQ(m.RemainingTurns(p, "refresh"), 2);
}
TEST(PlannedBuff, ExplicitStackAddsMagnitude) {
  Player p("P", 30, 42);
  BuffManager m;
  BuffSpec s{};
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  s.kind = BuffKind::kDamage;
  s.flat = 2;
  s.turns = 2;
  s.stacking = StackingRule::kStack;
  m.Add(p, "stack", s);
  m.Add(p, "stack", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 14);
}
TEST(PlannedBuff, UsesConsumedPerMatchingCardNotPerEffect) {
  Player p("P", 30, 42);
  BuffManager m;
  BuffSpec s{};
  Card c("F", "", CardRarity::kCommon, CardType::kAttack, 1, 0);
  s.kind = BuffKind::kDamage;
  s.flat = 3;
  s.uses = 2;
  m.Add(p, "uses", s);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 13);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 5), 8);
  m.OnCardPlayed(p, c);
  EXPECT_EQ(m.RemainingUses(p, "uses"), 1);
  m.OnCardPlayed(p, c);
  EXPECT_EQ(m.RemainingUses(p, "uses"), 0);
  EXPECT_EQ(m.Value(p, c, BuffKind::kDamage, 10), 10);
}
TEST(PlannedBuff, InstanceBuffDoesNotAffectSameNameCopy) {
  Player p("P", 30, 42);
  BuffManager m;
  BuffSpec s{};
  Card a("Same", "", CardRarity::kCommon, CardType::kAttack, 1, 0), b(a);
  s.kind = BuffKind::kDamage;
  s.flat = 3;
  s.turns = 2;
  s.card = &a;
  m.Add(p, "specific", s);
  EXPECT_EQ(m.Value(p, a, BuffKind::kDamage, 10), 13);
  EXPECT_EQ(m.Value(p, b, BuffKind::kDamage, 10), 10);
}
