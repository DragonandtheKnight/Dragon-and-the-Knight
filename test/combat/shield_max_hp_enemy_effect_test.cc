#include "combat/skill_cards/shield_max_hp_enemy_effect.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"

namespace {
constexpr std::uint32_t kSeed = 42;
}  // namespace

TEST(ShieldMaxHpEnemyEffectTest, ShieldsTheLivingEnemyWithTheHighestMaxHp) {
  Goblin caster(1, kSeed);   // max hp 30, casts the skill
  Goblin dead_king(3, kSeed);  // max hp 50, dead: ignored
  Goblin goblin(2, kSeed);   // max hp 40: wins
  EmptyEnemy empty(0, kSeed);
  dead_king.TakeDamage(1000);
  const ShieldMaxHpEnemyEffect effect(5);

  std::vector<Enemy*> enemies = {&caster, &dead_king, &goblin, &empty};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  effect.Apply(context);

  EXPECT_EQ(goblin.GetShield(), 5);
  EXPECT_EQ(caster.GetShield(), 0);
  EXPECT_EQ(dead_king.GetShield(), 0);
}

TEST(ShieldMaxHpEnemyEffectTest, SkipsTheCasterWhenItHasTheHighestMaxHp) {
  Goblin caster(3, kSeed);  // max hp 50, casts: skipped
  Goblin small(1, kSeed);  // max hp 30
  Goblin large(2, kSeed);  // max hp 40: wins among the others
  const ShieldMaxHpEnemyEffect effect(5);

  std::vector<Enemy*> enemies = {&caster, &small, &large};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  effect.Apply(context);

  EXPECT_EQ(large.GetShield(), 5);
  EXPECT_EQ(small.GetShield(), 0);
  EXPECT_EQ(caster.GetShield(), 0);
}

TEST(ShieldMaxHpEnemyEffectTest, BreaksATieBySlotOrder) {
  Goblin caster(2, kSeed);  // max hp 40, casts: skipped
  Goblin first(1, kSeed);    // max hp 30, earlier slot
  Goblin second(1, kSeed);   // max hp 30, later slot
  const ShieldMaxHpEnemyEffect effect(5);

  std::vector<Enemy*> enemies = {&first, &second, &caster};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  effect.Apply(context);

  EXPECT_EQ(first.GetShield(), 5);
  EXPECT_EQ(second.GetShield(), 0);
}

TEST(ShieldMaxHpEnemyEffectTest, DoesNothingWhenTheContextHasNoEnemies) {
  Goblin caster(1, kSeed);
  const ShieldMaxHpEnemyEffect effect(5);

  EffectContext context;
  context.target = &caster;
  context.enemies = nullptr;
  effect.Apply(context);
}

TEST(ShieldMaxHpEnemyEffectTest, DoesNothingWhenEveryOtherSlotIsDead) {
  Goblin caster(1, kSeed);
  EmptyEnemy empty(0, kSeed);
  const ShieldMaxHpEnemyEffect effect(5);

  std::vector<Enemy*> enemies = {&caster, &empty, &empty};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  effect.Apply(context);

  EXPECT_EQ(caster.GetShield(), 0);
}

TEST(ShieldMaxHpEnemyEffectTest, CloneCarriesTheAmount) {
  const ShieldMaxHpEnemyEffect effect(5);
  const std::unique_ptr<CardEffect> copy = effect.Clone();

  Goblin caster(1, kSeed);
  Goblin goblin(2, kSeed);
  EmptyEnemy empty(0, kSeed);
  std::vector<Enemy*> enemies = {&caster, &goblin, &empty};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  ASSERT_NE(copy, nullptr);
  copy->Apply(context);

  EXPECT_EQ(goblin.GetShield(), 5);
}

TEST(ShieldMaxHpEnemyEffectTest, RunsWhenTheCardCarriesIt) {
  Goblin caster(1, kSeed);
  Goblin goblin(2, kSeed);
  EmptyEnemy empty(0, kSeed);
  Card card("Shaman ward", "Wards the heartiest ally", CardRarity::kCommon,
            CardType::kDefense, 1, 5);
  card.AddEffect(std::make_unique<ShieldMaxHpEnemyEffect>(5));

  std::vector<Enemy*> enemies = {&caster, &goblin, &empty};
  EffectContext context;
  context.target = &caster;
  context.enemies = &enemies;
  card.ApplyEffect(context);

  EXPECT_EQ(goblin.GetShield(), 5);
}
