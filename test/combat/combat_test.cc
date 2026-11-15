#include "combat/combat.h"

#include <cstdint>
#include <vector>

#include "enemy/empty_enemy.h"
#include "enemy/enemy_skill.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;
}  // namespace

TEST(CombatConstructorTest, ExposesSlotsInOrder) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  EXPECT_EQ(combat.GetEnemy(0), &goblin);
  EXPECT_EQ(combat.GetEnemy(1), &empty);
  EXPECT_EQ(combat.GetEnemy(2), &goblin);
  EXPECT_EQ(combat.GetEnemy(3), &empty);
}

TEST(CombatConstructorTest, StartsWithZeroAp) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_EQ(combat.GetCurrentAp(), 0);
  EXPECT_EQ(combat.GetNextAp(), 0);
}

TEST(CombatConstructorTest, EmptySlotIsDead) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  EXPECT_TRUE(combat.GetEnemy(1)->IsEmpty());
  EXPECT_FALSE(combat.GetEnemy(1)->IsAlive());
}

TEST(CombatConstructorTest, NextActsStartAsAlreadyDead) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  for (int slot = 0; slot < Combat::kMaxEnemySlots; ++slot) {
    EXPECT_EQ(combat.GetNextAct(slot), EnemySkill::kAlreadyDead);
  }
}

TEST(CombatIsOverTest, IsNotOverWhileAnEnemyLives) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_FALSE(combat.IsOver());
}

TEST(CombatIsOverTest, IsOverWhenEverySlotIsEmpty) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_TRUE(combat.IsOver());
}

TEST(CombatIsOverTest, IsOverWhenThePlayerDies) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  player.TakeDamage(1000);

  EXPECT_TRUE(combat.IsOver());
}
