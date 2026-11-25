#include "combat/skill_cards/heal_enemies_effect.h"

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

TEST(HealEnemiesEffectTest, HealsEachEnemyByItsOwnMaxHpPercent) {
  Goblin goblin_one(1, kSeed);  // max hp 30, 20% = 6
  Goblin goblin_two(2, kSeed);  // max hp 40, 20% = 8
  EmptyEnemy empty(0, kSeed);
  goblin_one.TakeDamage(9);
  goblin_two.TakeDamage(9);
  const HealEnemiesEffect effect(20);

  std::vector<Enemy*> enemies = {&goblin_one, &goblin_two, &empty};
  EffectContext context;
  context.target = &goblin_one;
  context.enemies = &enemies;
  effect.Apply(context);

  EXPECT_EQ(goblin_one.GetHp(), 27);
  EXPECT_EQ(goblin_two.GetHp(), 39);
}

TEST(HealEnemiesEffectTest, KeepsTheAmountAcrossTwoUses) {
  Goblin goblin(1, kSeed);
  goblin.TakeDamage(20);
  const HealEnemiesEffect effect(20);

  std::vector<Enemy*> enemies = {&goblin};
  EffectContext context;
  context.enemies = &enemies;
  effect.Apply(context);
  effect.Apply(context);

  EXPECT_EQ(goblin.GetHp(), 22);
}

TEST(HealEnemiesEffectTest, DoesNothingWhenTheContextHasNoEnemies) {
  const HealEnemiesEffect effect(20);

  EffectContext context;
  context.enemies = nullptr;
  effect.Apply(context);
}

TEST(HealEnemiesEffectTest, CloneCarriesTheAmount) {
  const HealEnemiesEffect effect(20);
  const std::unique_ptr<CardEffect> copy = effect.Clone();

  Goblin goblin(1, kSeed);
  goblin.TakeDamage(20);
  std::vector<Enemy*> enemies = {&goblin};
  EffectContext context;
  context.enemies = &enemies;
  ASSERT_NE(copy, nullptr);
  copy->Apply(context);

  EXPECT_EQ(goblin.GetHp(), 16);
}

TEST(HealEnemiesEffectTest, RunsWhenTheCardCarriesIt) {
  Goblin goblin(1, kSeed);
  goblin.TakeDamage(20);
  Card card("Shaman group mend", "Mends every ally", CardRarity::kCommon,
            CardType::kHeal, 1, 5);
  card.AddEffect(std::make_unique<HealEnemiesEffect>(20));

  std::vector<Enemy*> enemies = {&goblin};
  EffectContext context;
  context.enemies = &enemies;
  card.ApplyEffect(context);

  EXPECT_EQ(goblin.GetHp(), 16);
}
