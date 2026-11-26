// Opt-in tests for the shared effect interface.

#include "card/block_effect.h"
#include "card/damage_effect.h"
#include "card/effect_context.h"
#include "card/heal_effect.h"
#include "card/target_type.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"
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
TEST(PlannedTarget, PlayerReceivesExplicitSelfBlock) {
  Player player("Knight", 30, 42);
  player.AddShield(2);
  EffectContext context;
  context.caster_player = &player;
  context.target_player = &player;

  BlockEffect(6, TargetType::kSelf).Apply(context);

  EXPECT_EQ(player.GetShield(), 8);
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
  EmptyEnemy empty(0, 45);
  e.TakeDamage(10);
  ally.TakeDamage(10);
  dead.TakeDamage(100);
  EffectContext c{};
  c.caster_enemy = &e;
  c.enemy_targets = {&e, &ally, &dead, &empty, nullptr};
  HealEffect(5, TargetType::kAllAllies).Apply(c);
  EXPECT_EQ(e.GetHp(), 25);
  EXPECT_EQ(ally.GetHp(), 25);
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_EQ(empty.GetHp(), 0);
}
TEST(PlannedTarget, CloneOwnsCopyAndPreservesScope) {
  DamageEffect effect(5, TargetType::kAllEnemies);
  auto copy = effect.Clone();
  ASSERT_NE(copy, nullptr);
  EXPECT_NE(copy.get(), &effect);
  EXPECT_EQ(copy->GetTargetType(), TargetType::kAllEnemies);

  Player player("Knight", 30, 42);
  Goblin enemy(1, 42);
  EffectContext context;
  context.caster_player = &player;
  context.enemy_targets = {&enemy};
  copy->Apply(context);
  EXPECT_EQ(enemy.GetHp(), 25);
}

TEST(PlannedTarget, ExplicitTargetDoesNotAlsoUseLegacyTarget) {
  Player player("Knight", 30, 42);
  Goblin selected(1, 42);
  Goblin legacy(1, 43);
  EffectContext context;
  context.player = &player;
  context.target = &legacy;
  context.caster_player = &player;
  context.target_enemy = &selected;

  DamageEffect(8, TargetType::kSingleEnemy).Apply(context);

  EXPECT_EQ(selected.GetHp(), 22);
  EXPECT_EQ(legacy.GetHp(), 30);
  EXPECT_EQ(player.GetHp(), 30);
}

TEST(PlannedTarget, ExplicitTargetRejectsMissingAndWrongSideRecipients) {
  Player player("Knight", 30, 42);
  Goblin legacy(1, 42);
  EffectContext context;
  context.caster_player = &player;
  context.target = &legacy;
  DamageEffect damage(8, TargetType::kSingleEnemy);

  damage.Apply(context);
  context.target_player = &player;
  damage.Apply(context);

  EXPECT_EQ(player.GetHp(), 30);
  EXPECT_EQ(legacy.GetHp(), 30);
}

TEST(PlannedTarget, PlayerDamagesLivingEnemiesThroughTheirShields) {
  Player player("Knight", 30, 42);
  Goblin shielded(1, 42);
  Goblin unshielded(1, 43);
  Goblin dead(1, 44);
  EmptyEnemy empty(0, 45);
  shielded.AddShield(3);
  dead.TakeDamage(100);
  EffectContext context;
  context.caster_player = &player;
  context.enemy_targets = {&shielded, &empty, &unshielded, &dead, nullptr};

  DamageEffect(8, TargetType::kAllEnemies).Apply(context);

  EXPECT_EQ(shielded.GetHp(), 25);
  EXPECT_EQ(shielded.GetShield(), 0);
  EXPECT_EQ(unshielded.GetHp(), 22);
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_EQ(empty.GetHp(), 0);
  EXPECT_EQ(player.GetHp(), 30);
}

TEST(PlannedTarget, GroupShieldStacksAndSkipsDeadAndEmptyEnemies) {
  Goblin caster(1, 42);
  Goblin ally(1, 43);
  Goblin dead(1, 44);
  EmptyEnemy empty(0, 45);
  dead.TakeDamage(100);
  ally.AddShield(2);
  EffectContext context;
  context.caster_enemy = &caster;
  context.enemy_targets = {&caster, &ally, &dead, &empty, nullptr};

  BlockEffect(6, TargetType::kAllAllies).Apply(context);

  EXPECT_EQ(caster.GetShield(), 6);
  EXPECT_EQ(ally.GetShield(), 8);
  EXPECT_EQ(dead.GetShield(), 0);
  EXPECT_EQ(empty.GetShield(), 0);
}

TEST(PlannedTarget, ExplicitHealCapsHpAndCannotRevive) {
  Goblin caster(1, 42);
  Goblin ally(1, 43);
  Goblin dead(1, 44);
  ally.TakeDamage(2);
  dead.TakeDamage(100);
  EffectContext context;
  context.caster_enemy = &caster;
  context.enemy_targets = {&ally, &dead};

  HealEffect(9, TargetType::kAllAllies).Apply(context);

  EXPECT_EQ(ally.GetHp(), ally.GetMaxHp());
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_FALSE(dead.IsAlive());
}

TEST(PlannedTarget, ClonedHealAndBlockKeepAmountsAndTargetTypes) {
  Goblin caster(1, 42);
  Goblin ally(1, 43);
  ally.TakeDamage(10);
  EffectContext context;
  context.caster_enemy = &caster;
  context.target_enemy = &ally;
  auto heal = HealEffect(5, TargetType::kSingleAlly).Clone();
  auto block = BlockEffect(6, TargetType::kSingleAlly).Clone();

  ASSERT_NE(heal, nullptr);
  ASSERT_NE(block, nullptr);
  EXPECT_EQ(heal->GetTargetType(), TargetType::kSingleAlly);
  EXPECT_EQ(block->GetTargetType(), TargetType::kSingleAlly);
  heal->Apply(context);
  block->Apply(context);
  EXPECT_EQ(ally.GetHp(), 25);
  EXPECT_EQ(ally.GetShield(), 6);
  EXPECT_EQ(caster.GetShield(), 0);
}
