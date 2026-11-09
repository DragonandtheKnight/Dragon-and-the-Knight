// Tests for Enemy, the base class every monster derives from.
//
// Enemy is abstract - calMove() is pure virtual - so these tests build it
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

#include "enemy/Enemy.h"

#include <cstdint>

#include "gtest/gtest.h"

namespace {

// The smallest Enemy that can exist: it forwards the constructor and returns
// one fixed skill, which is all the base-class tests need from calMove().
class StubEnemy : public Enemy {
 public:
  StubEnemy(Enemy_type type, int max_hp, int difficulty, int level,
            std::uint32_t seed)
      : Enemy(type, max_hp, difficulty, level, seed) {}

  Enemy_skill calMove() override { return Enemy_skill::Slime_slap_lv1; }
};

}  // namespace

TEST(EnemyTest, StartsAtFullHealth) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  EXPECT_EQ(enemy.get_hp(), 30);
}

TEST(EnemyTest, StartsWithoutShield) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  EXPECT_EQ(enemy.get_shield(), 0);
}

TEST(EnemyTest, StartsAlive) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  EXPECT_TRUE(enemy.is_alive());
}

TEST(EnemyTest, ConstructorStoresTypeDifficultyAndLevel) {
  StubEnemy enemy(Enemy_type::Goblin, 25, 2, 2, 7);

  EXPECT_EQ(enemy.get_name(), Enemy_type::Goblin);
  EXPECT_EQ(enemy.get_difficulty(), 2);
  EXPECT_EQ(enemy.get_level(), 2);
}

TEST(EnemyTest, DealDmgTakesHpAway) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.deal_dmg(10);

  EXPECT_EQ(enemy.get_hp(), 20);
}

// Hp is a life counter, not a number line: a hit bigger than the remaining hp
// must stop at zero so the UI never shows a negative bar.
TEST(EnemyTest, DealDmgStopsAtZeroInsteadOfGoingNegative) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.deal_dmg(100);

  EXPECT_EQ(enemy.get_hp(), 0);
}

TEST(EnemyTest, DealDmgKillsWhenHpReachesZero) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.deal_dmg(30);

  EXPECT_EQ(enemy.get_hp(), 0);
  EXPECT_FALSE(enemy.is_alive());
}

TEST(EnemyTest, DealDmgLeavesTheEnemyAliveWhileHpIsLeft) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.deal_dmg(29);

  EXPECT_EQ(enemy.get_hp(), 1);
  EXPECT_TRUE(enemy.is_alive());
}

TEST(EnemyTest, ShieldTakesTheDamageFirst) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.add_shield(5);

  enemy.deal_dmg(3);

  EXPECT_EQ(enemy.get_shield(), 2);
  EXPECT_EQ(enemy.get_hp(), 30);
}

TEST(EnemyTest, DamagePastTheShieldReachesHp) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.add_shield(5);

  enemy.deal_dmg(8);

  EXPECT_EQ(enemy.get_shield(), 0);
  EXPECT_EQ(enemy.get_hp(), 27);
}

TEST(EnemyTest, ShieldStopsAtZeroInsteadOfGoingNegative) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.add_shield(5);

  enemy.deal_dmg(20);

  EXPECT_EQ(enemy.get_shield(), 0);
  EXPECT_EQ(enemy.get_hp(), 15);
}

// Two block cards in one round add up, they do not replace each other.
TEST(EnemyTest, ShieldFromTwoCardsPilesUp) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.add_shield(3);
  enemy.add_shield(4);

  EXPECT_EQ(enemy.get_shield(), 7);
}

TEST(EnemyTest, ClearShieldWipesItToZero) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.add_shield(5);

  enemy.clear_shield();

  EXPECT_EQ(enemy.get_shield(), 0);
}

TEST(EnemyTest, ClearShieldLeavesHpAlone) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.add_shield(5);
  enemy.deal_dmg(3);

  enemy.clear_shield();

  EXPECT_EQ(enemy.get_shield(), 0);
  EXPECT_EQ(enemy.get_hp(), 30);
}

// The battle code may still hit a corpse once; that must not bring it back.
TEST(EnemyTest, ADefeatedEnemyIgnoresFurtherDamage) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.deal_dmg(30);

  enemy.deal_dmg(10);

  EXPECT_EQ(enemy.get_hp(), 0);
  EXPECT_FALSE(enemy.is_alive());
}

// One enemy taking damage must not touch the next one on the same floor.
TEST(EnemyTest, TwoEnemiesDoNotShareState) {
  StubEnemy first(Enemy_type::Slime, 30, 1, 1, 7);
  StubEnemy second(Enemy_type::Slime, 30, 1, 1, 7);

  first.deal_dmg(3);
  first.add_shield(9);

  EXPECT_EQ(second.get_hp(), 30);
  EXPECT_EQ(second.get_shield(), 0);
}

// The battle code only ever holds an Enemy*, so the skill has to come back
// through the base type.
TEST(EnemyTest, CalMoveIsDispatchedThroughTheBaseType) {
  StubEnemy stub(Enemy_type::Slime, 30, 1, 1, 7);
  Enemy& base = stub;

  EXPECT_EQ(base.calMove(), Enemy_skill::Slime_slap_lv1);
}

TEST(EnemyTest, HealRestoresLostHp) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.deal_dmg(10);

  enemy.heal(4);

  EXPECT_EQ(enemy.get_hp(), 24);
}

TEST(EnemyTest, HealCapsAtMaxHp) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.deal_dmg(10);

  enemy.heal(100);

  EXPECT_EQ(enemy.get_hp(), 30);
}

TEST(EnemyTest, HealAtFullHealthDoesNotOverflow) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);

  enemy.heal(10);

  EXPECT_EQ(enemy.get_hp(), 30);
}

// A dead monster stays dead: healing must not bring it back.
TEST(EnemyTest, ADefeatedEnemyCannotBeHealed) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.deal_dmg(30);

  enemy.heal(10);

  EXPECT_EQ(enemy.get_hp(), 0);
  EXPECT_FALSE(enemy.is_alive());
}

// Healing is a hp thing only; the shield of the round is untouched.
TEST(EnemyTest, HealDoesNotTouchShield) {
  StubEnemy enemy(Enemy_type::Slime, 30, 1, 1, 7);
  enemy.deal_dmg(10);
  enemy.add_shield(5);

  enemy.heal(3);

  EXPECT_EQ(enemy.get_hp(), 23);
  EXPECT_EQ(enemy.get_shield(), 5);
}

TEST(EnemyTest, GetMaxHpIsTheHpItStartedWith) {
  StubEnemy enemy(Enemy_type::Goblin, 30, 1, 1, 7);

  EXPECT_EQ(enemy.get_max_hp(), 30);
  EXPECT_EQ(enemy.get_hp(), 30);
}

// Damage lowers hp but never the ceiling: the ceiling is what healing clamps
// to, so it has to survive every hit.
TEST(EnemyTest, DamageDoesNotLowerMaxHp) {
  StubEnemy enemy(Enemy_type::Goblin, 30, 1, 1, 7);

  enemy.deal_dmg(20);

  EXPECT_EQ(enemy.get_hp(), 10);
  EXPECT_EQ(enemy.get_max_hp(), 30);
}
