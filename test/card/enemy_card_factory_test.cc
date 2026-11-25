// Planned tests (RED), disabled by default;
// Create(EnemySkill) -> Card is proposed.
// Exact skill values and kAlreadyDead handling remain unspecified.
#include "card/enemy_card_factory.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/card.h"
#include "card/effect_context.h"
#include "card/target_type.h"
#include "enemy/enemy_skill.h"
#include "enemy/goblin.h"
#include "player/player.h"
TEST(PlannedFactory, AttackCardDamagesPlayer) {
  Goblin e(1, 42);
  Player p("P", 100, 42);
  Card card = EnemyCardFactory::Create(EnemySkill::kGoblinHitLv1);
  EXPECT_EQ(card.GetEffectTargetTypes(),
            (std::vector<TargetType>{TargetType::kSingleEnemy}));
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_player = &p;
  card.ApplyEffect(c);
  EXPECT_LT(p.GetHp(), 100);
  EXPECT_EQ(e.GetHp(), 30);
}
TEST(PlannedFactory, HealingBenefitsEnemyCaster) {
  Goblin e(1, 42);
  e.TakeDamage(20);
  Card card = EnemyCardFactory::Create(EnemySkill::kSlimeHealLv1);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_enemy = &e;
  card.ApplyEffect(c);
  EXPECT_GT(e.GetHp(), 10);
  EXPECT_LE(e.GetHp(), 30);
}
TEST(PlannedFactory, GroupHealBenefitsLivingAlliesOnly) {
  Goblin e(1, 42), ally(1, 43), dead(1, 44);
  e.TakeDamage(20);
  ally.TakeDamage(20);
  dead.TakeDamage(100);
  Card card = EnemyCardFactory::Create(EnemySkill::kShamanHealAllLv1);
  EXPECT_EQ(card.GetEffectTargetTypes(),
            (std::vector<TargetType>{TargetType::kAllAllies}));
  EffectContext c{};
  c.caster_enemy = &e;
  c.enemy_targets = {&e, &ally, &dead};
  card.ApplyEffect(c);
  EXPECT_GT(e.GetHp(), 10);
  EXPECT_GT(ally.GetHp(), 10);
  EXPECT_EQ(dead.GetHp(), 0);
}
TEST(PlannedFactory, DefenseAddsEnemyShield) {
  Goblin e(1, 42);
  Card card = EnemyCardFactory::Create(EnemySkill::kGiantDefendLv1);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_enemy = &e;
  card.ApplyEffect(c);
  EXPECT_GT(e.GetShield(), 0);
}
TEST(PlannedFactory, CopiedCardOwnsIndependentEffects) {
  Card a = EnemyCardFactory::Create(EnemySkill::kGoblinHitLv1), b(a);
  a = EnemyCardFactory::Create(EnemySkill::kSlimeHealLv1);
  Goblin e(1, 42);
  Player p("P", 100, 42);
  EffectContext c{};
  c.caster_enemy = &e;
  c.target_player = &p;
  b.ApplyEffect(c);
  EXPECT_LT(p.GetHp(), 100);
}
