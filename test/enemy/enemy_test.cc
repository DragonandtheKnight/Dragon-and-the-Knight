// Tests for Enemy, the base class every monster derives from.
//
// Enemy is abstract - CalculateMove() is pure virtual - so these tests build it
// through a three-line stub and only look at the state Enemy itself owns:
// name, hp, shield, difficulty, level. Anything a subclass decides on its own
// (the stats of a level, which skill comes next) is tested next to that
// subclass, in goblin_test.cc.
//
// The damage rules are the ones the team settled on: a hit empties the shield
// first, whatever is left goes to hp, hp stops at zero instead of going
// negative, and an enemy whose hp reached zero is dead. Shield is thrown away
// at the end of every round, so it is added while the round runs and cleared
// when the round ends.

#include "enemy/enemy.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace {

// The smallest Enemy that can exist: it forwards the constructor and returns
// one fixed skill, which is all the base-class tests need from CalculateMove().
class StubEnemy : public Enemy {
 public:
  StubEnemy(EnemyType type, int max_hp, int difficulty, int level,
            std::uint32_t seed)
      : Enemy(type, max_hp, difficulty, level, seed) {}

  EnemySkill CalculateMove() override { return EnemySkill::kSlimeSlapLv1; }
};

}  // namespace

TEST(EnemyTest, StartsAtFullHealth) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  EXPECT_EQ(enemy.GetHp(), 30);
}

TEST(EnemyTest, StartsWithoutShield) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  EXPECT_EQ(enemy.GetShield(), 0);
}

TEST(EnemyTest, StartsAlive) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  EXPECT_TRUE(enemy.IsAlive());
}

TEST(EnemyTest, ConstructorStoresTypeDifficultyAndLevel) {
  StubEnemy enemy(EnemyType::kGoblin, 25, 2, 2, 7);

  EXPECT_EQ(enemy.GetName(), EnemyType::kGoblin);
  EXPECT_EQ(enemy.GetDifficulty(), 2);
  EXPECT_EQ(enemy.GetLevel(), 2);
}

TEST(EnemyTest, TakeDamageTakesHpAway) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.TakeDamage(10);

  EXPECT_EQ(enemy.GetHp(), 20);
}

// Hp is a life counter, not a number line: a hit bigger than the remaining hp
// must stop at zero so the UI never shows a negative bar.
TEST(EnemyTest, TakeDamageStopsAtZeroInsteadOfGoingNegative) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.TakeDamage(100);

  EXPECT_EQ(enemy.GetHp(), 0);
}

TEST(EnemyTest, TakeDamageKillsWhenHpReachesZero) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.TakeDamage(30);

  EXPECT_EQ(enemy.GetHp(), 0);
  EXPECT_FALSE(enemy.IsAlive());
}

TEST(EnemyTest, TakeDamageLeavesTheEnemyAliveWhileHpIsLeft) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.TakeDamage(29);

  EXPECT_EQ(enemy.GetHp(), 1);
  EXPECT_TRUE(enemy.IsAlive());
}

TEST(EnemyTest, ShieldTakesTheDamageFirst) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.AddShield(5);

  enemy.TakeDamage(3);

  EXPECT_EQ(enemy.GetShield(), 2);
  EXPECT_EQ(enemy.GetHp(), 30);
}

TEST(EnemyTest, DamagePastTheShieldReachesHp) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.AddShield(5);

  enemy.TakeDamage(8);

  EXPECT_EQ(enemy.GetShield(), 0);
  EXPECT_EQ(enemy.GetHp(), 27);
}

TEST(EnemyTest, ShieldStopsAtZeroInsteadOfGoingNegative) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.AddShield(5);

  enemy.TakeDamage(20);

  EXPECT_EQ(enemy.GetShield(), 0);
  EXPECT_EQ(enemy.GetHp(), 15);
}

// Two block cards in one round add up, they do not replace each other.
TEST(EnemyTest, ShieldFromTwoCardsPilesUp) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.AddShield(3);
  enemy.AddShield(4);

  EXPECT_EQ(enemy.GetShield(), 7);
}

TEST(EnemyTest, ClearShieldWipesItToZero) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.AddShield(5);

  enemy.ClearShield();

  EXPECT_EQ(enemy.GetShield(), 0);
}

TEST(EnemyTest, ClearShieldLeavesHpAlone) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.AddShield(5);
  enemy.TakeDamage(3);

  enemy.ClearShield();

  EXPECT_EQ(enemy.GetShield(), 0);
  EXPECT_EQ(enemy.GetHp(), 30);
}

// The battle code may still hit a corpse once; that must not bring it back.
TEST(EnemyTest, ADefeatedEnemyIgnoresFurtherDamage) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.TakeDamage(30);

  enemy.TakeDamage(10);

  EXPECT_EQ(enemy.GetHp(), 0);
  EXPECT_FALSE(enemy.IsAlive());
}

// One enemy taking damage must not touch the next one on the same floor.
TEST(EnemyTest, TwoEnemiesDoNotShareState) {
  StubEnemy first(EnemyType::kSlime, 30, 1, 1, 7);
  StubEnemy second(EnemyType::kSlime, 30, 1, 1, 7);

  first.TakeDamage(3);
  first.AddShield(9);

  EXPECT_EQ(second.GetHp(), 30);
  EXPECT_EQ(second.GetShield(), 0);
}

// The battle code only ever holds an Enemy*, so the skill has to come back
// through the base type.
TEST(EnemyTest, CalculateMoveIsDispatchedThroughTheBaseType) {
  StubEnemy stub(EnemyType::kSlime, 30, 1, 1, 7);
  Enemy& base = stub;

  EXPECT_EQ(base.CalculateMove(), EnemySkill::kSlimeSlapLv1);
}

TEST(EnemyTest, HealRestoresLostHp) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.TakeDamage(10);

  enemy.Heal(4);

  EXPECT_EQ(enemy.GetHp(), 24);
}

TEST(EnemyTest, HealCapsAtMaxHp) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.TakeDamage(10);

  enemy.Heal(100);

  EXPECT_EQ(enemy.GetHp(), 30);
}

TEST(EnemyTest, HealAtFullHealthDoesNotOverflow) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);

  enemy.Heal(10);

  EXPECT_EQ(enemy.GetHp(), 30);
}

// A dead monster stays dead: healing must not bring it back.
TEST(EnemyTest, ADefeatedEnemyCannotBeHealed) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.TakeDamage(30);

  enemy.Heal(10);

  EXPECT_EQ(enemy.GetHp(), 0);
  EXPECT_FALSE(enemy.IsAlive());
}

// Healing is a hp thing only; the shield of the round is untouched.
TEST(EnemyTest, HealDoesNotTouchShield) {
  StubEnemy enemy(EnemyType::kSlime, 30, 1, 1, 7);
  enemy.TakeDamage(10);
  enemy.AddShield(5);

  enemy.Heal(3);

  EXPECT_EQ(enemy.GetHp(), 23);
  EXPECT_EQ(enemy.GetShield(), 5);
}

TEST(EnemyTest, GetMaxHpIsTheHpItStartedWith) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);

  EXPECT_EQ(enemy.GetMaxHp(), 30);
  EXPECT_EQ(enemy.GetHp(), 30);
}

// Damage lowers hp but never the ceiling: the ceiling is what healing clamps
// to, so it has to survive every hit.
TEST(EnemyTest, DamageDoesNotLowerMaxHp) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);

  enemy.TakeDamage(20);

  EXPECT_EQ(enemy.GetHp(), 10);
  EXPECT_EQ(enemy.GetMaxHp(), 30);
}

// A negative amount is a caller bug, not a way to run the rules backwards:
// every entry point ignores it instead of healing or padding the target.
TEST(EnemyTest, TakeDamageIgnoresNegativeAmounts) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);

  enemy.TakeDamage(-5);

  EXPECT_EQ(enemy.GetHp(), 30);
  EXPECT_EQ(enemy.GetShield(), 0);
}

TEST(EnemyTest, HealIgnoresNegativeAmounts) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);

  enemy.Heal(-40);

  EXPECT_EQ(enemy.GetHp(), 30);
  EXPECT_TRUE(enemy.IsAlive());
}

TEST(EnemyTest, AddShieldIgnoresNegativeAmounts) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);

  enemy.AddShield(-10);

  EXPECT_EQ(enemy.GetShield(), 0);
}

// The combination the review found: a negative shield must not turn the next
// ordinary hit into a bigger one.
TEST(EnemyTest, NegativeShieldDoesNotAmplifyTheNextHit) {
  StubEnemy enemy(EnemyType::kGoblin, 30, 1, 1, 7);
  enemy.AddShield(-10);

  enemy.TakeDamage(5);

  EXPECT_EQ(enemy.GetHp(), 25);
  EXPECT_EQ(enemy.GetShield(), 0);
}
