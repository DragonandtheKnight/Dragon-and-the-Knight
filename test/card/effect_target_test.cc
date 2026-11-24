// Planned tests (RED), disabled by default; APIs are not implemented.
// Assumption: effect constructors accept (amount, TargetType).
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/block_effect.h"
#include "card/card.h"
#include "card/damage_effect.h"
#include "card/effect_context.h"
#include "card/heal_effect.h"
#include "card/target_type.h"
#include "enemy/goblin.h"
#include "player/player.h"
TEST(PlannedTarget, EnemyDamagesPlayer) {
  Player p("P", 30, 42);
  Goblin e(1, 42);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_player = &p;
  DamageEffect(8, TargetType::kSingleEnemy).Apply(c);
  EXPECT_EQ(p.GetHp(), 22);
  EXPECT_EQ(e.GetHp(), 30);
}
TEST(PlannedTarget, PlayerDamagesEnemy) {
  Player p("P", 30, 42);
  Goblin e(1, 42);
  EffectContext c{};
  c.caster_player = &p;
  c.target_enemy = &e;
  DamageEffect(8, TargetType::kSingleEnemy).Apply(c);
  EXPECT_EQ(p.GetHp(), 30);
  EXPECT_EQ(e.GetHp(), 22);
}
TEST(PlannedTarget, SingleAllyAllowsEnemySelfHealing) {
  Goblin e(1, 42);
  e.TakeDamage(10);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_enemy = &e;
  HealEffect(5, TargetType::kSingleAlly).Apply(c);
  EXPECT_EQ(e.GetHp(), 25);
}
TEST(PlannedTarget, EnemyReceivesSelfBlock) {
  Goblin e(1, 42);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_enemy = &e;
  BlockEffect(6, TargetType::kSelf).Apply(c);
  EXPECT_EQ(e.GetShield(), 6);
}
TEST(PlannedTarget, AllEnemiesIsRelativeToEnemyCaster) {
  Player p("P", 30, 42);
  Goblin e(1, 42);
  EffectContext c{};
  c.caster_enemy = &e;
  c.player_targets = {&p};
  DamageEffect(4, TargetType::kAllEnemies).Apply(c);
  EXPECT_EQ(p.GetHp(), 26);
  EXPECT_EQ(e.GetHp(), 30);
}
TEST(PlannedTarget, AllAlliesSkipsDeadEnemies) {
  Goblin e(1, 42), ally(1, 43), dead(1, 44);
  e.TakeDamage(10);
  ally.TakeDamage(10);
  dead.TakeDamage(100);
  EffectContext c{};
  c.caster_enemy = &e;
  c.enemy_targets = {&e, &ally, &dead};
  HealEffect(5, TargetType::kAllAllies).Apply(c);
  EXPECT_EQ(e.GetHp(), 25);
  EXPECT_EQ(ally.GetHp(), 25);
  EXPECT_EQ(dead.GetHp(), 0);
}
TEST(PlannedTarget, CloneOwnsCopyAndPreservesScope) {
  DamageEffect effect(5, TargetType::kAllEnemies);
  auto copy = effect.Clone();
  ASSERT_NE(copy, nullptr);
  EXPECT_NE(copy.get(), &effect);
  EXPECT_EQ(copy->GetTargetType(), TargetType::kAllEnemies);
}
