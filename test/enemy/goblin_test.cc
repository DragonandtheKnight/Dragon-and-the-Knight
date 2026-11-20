// Tests for Goblin: the stats each level gets, and which skill CalculateMove()
// picks.
//
// The roll comes from the Rng handed to the constructor, so a fixed seed makes
// a run reproducible and the same seed always replays the same fight. The
// checks that involve chance still count a thousand rolls and allow a
// tolerance about six standard deviations wide: std::uniform_int_distribution
// is allowed to map the engine differently in each standard library, so the
// exact move at a given position is not portable and must not be asserted.
//
// Level 3 alternates: a dash is always paid back by exactly one dash_attack,
// and that comeback does not use up a roll. Both halves are checked below.

#include "enemy/goblin.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 1000;

// 1000 rolls, so 80 is about six standard deviations at p = 0.3 and seven at
// p = 0.7. A correct goblin essentially cannot fail this.
constexpr int kTolerance = 80;

// One seed for every test, so a failure can be reproduced exactly.
constexpr std::uint32_t kSeed = 20261108;

std::vector<EnemySkill> RollMoves(Goblin& goblin, int count) {
  std::vector<EnemySkill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(goblin.CalculateMove());
  }
  return moves;
}

int CountOf(const std::vector<EnemySkill>& moves, EnemySkill skill) {
  return static_cast<int>(std::count(moves.begin(), moves.end(), skill));
}

// The move right after a dash is forced, so those moves say nothing about the
// odds; this keeps only the moves that came out of an actual roll.
std::vector<EnemySkill> RollsOnly(const std::vector<EnemySkill>& moves) {
  std::vector<EnemySkill> rolls;
  for (std::size_t i = 0; i < moves.size(); ++i) {
    rolls.push_back(moves[i]);
    if (moves[i] == EnemySkill::kGoblinDashLv3) {
      ++i;  // Skip the dash_attack that this dash just forced.
    }
  }
  return rolls;
}

}  // namespace

TEST(GoblinTest, LevelOneStats) {
  Goblin goblin(1, kSeed);

  EXPECT_EQ(goblin.GetName(), EnemyType::kGoblin);
  EXPECT_EQ(goblin.GetHp(), 30);
  EXPECT_EQ(goblin.GetDifficulty(), 1);
  EXPECT_EQ(goblin.GetLevel(), 1);
}

TEST(GoblinTest, LevelTwoStats) {
  Goblin goblin(2, kSeed);

  EXPECT_EQ(goblin.GetName(), EnemyType::kGoblinCaptain);
  EXPECT_EQ(goblin.GetHp(), 40);
  EXPECT_EQ(goblin.GetDifficulty(), 2);
  EXPECT_EQ(goblin.GetLevel(), 2);
}

TEST(GoblinTest, LevelThreeStats) {
  Goblin goblin(3, kSeed);

  EXPECT_EQ(goblin.GetName(), EnemyType::kGoblinKing);
  EXPECT_EQ(goblin.GetHp(), 50);
  EXPECT_EQ(goblin.GetDifficulty(), 3);
  EXPECT_EQ(goblin.GetLevel(), 3);
}

// The map picks the level, so a level the stats table does not know about
// still has to produce a working goblin instead of a broken one.
TEST(GoblinTest, LevelOutsideOneToThreeFallsBackToTheLevelOneGoblin) {
  Goblin goblin(4, kSeed);

  EXPECT_EQ(goblin.GetName(), EnemyType::kGoblin);
  EXPECT_EQ(goblin.GetHp(), 30);
  EXPECT_EQ(goblin.GetDifficulty(), 1);
  EXPECT_EQ(goblin.GetLevel(), 4);

  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);
  EXPECT_EQ(CountOf(moves, EnemySkill::kGoblinHitLv1), kRolls);
}

// The map hands each monster a seed from its own stream. The same seed has to
// replay the same fight, and a different seed has to give a different one -
// otherwise the constructor is not really seeding anything.
TEST(GoblinTest, SameSeedReplaysTheSameMoves) {
  Goblin first(3, 12345);
  Goblin second(3, 12345);

  EXPECT_EQ(RollMoves(first, kRolls), RollMoves(second, kRolls));
}

TEST(GoblinTest, DifferentSeedsGiveDifferentMoves) {
  Goblin first(3, 12345);
  Goblin second(3, 54321);

  EXPECT_NE(RollMoves(first, kRolls), RollMoves(second, kRolls));
}

TEST(GoblinTest, LevelOneAlwaysUsesTheBasicHit) {
  Goblin goblin(1, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  EXPECT_EQ(CountOf(moves, EnemySkill::kGoblinHitLv1), kRolls);
}

TEST(GoblinTest, LevelTwoOnlyUsesItsTwoSkills) {
  Goblin goblin(2, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);
  const std::set<EnemySkill> used(moves.begin(), moves.end());
  const std::set<EnemySkill> allowed = {EnemySkill::kGoblinHitLv2,
                                        EnemySkill::kGoblinHardSlamLv2};

  EXPECT_EQ(used, allowed);
}

TEST(GoblinTest, LevelTwoBasicHitIsAboutSeventyPercent) {
  Goblin goblin(2, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  EXPECT_NEAR(CountOf(moves, EnemySkill::kGoblinHitLv2), kRolls * 70 / 100,
              kTolerance);
}

TEST(GoblinTest, LevelThreeOnlyUsesItsFourSkills) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);
  const std::set<EnemySkill> used(moves.begin(), moves.end());
  const std::set<EnemySkill> allowed = {
      EnemySkill::kGoblinHitLv3, EnemySkill::kGoblinHardSlamLv3,
      EnemySkill::kGoblinDashLv3, EnemySkill::kGoblinDashAttackLv3};

  EXPECT_EQ(used, allowed);
}

TEST(GoblinTest, LevelThreeDashIsAlwaysPaidBackByADashAttack) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  int dashes = 0;
  for (std::size_t i = 0; i + 1 < moves.size(); ++i) {
    if (moves[i] == EnemySkill::kGoblinDashLv3) {
      ++dashes;
      EXPECT_EQ(moves[i + 1], EnemySkill::kGoblinDashAttackLv3)
          << "the dash at index " << i << " was not paid back";
    }
  }

  EXPECT_GT(dashes, 0);
}

TEST(GoblinTest, LevelThreeNeverOpensWithADashAttack) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  ASSERT_FALSE(moves.empty());
  EXPECT_NE(moves.front(), EnemySkill::kGoblinDashAttackLv3);
}

TEST(GoblinTest, LevelThreeDashAttackNeverComesFromAnywhereElse) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  for (std::size_t i = 1; i < moves.size(); ++i) {
    if (moves[i] == EnemySkill::kGoblinDashAttackLv3) {
      EXPECT_EQ(moves[i - 1], EnemySkill::kGoblinDashLv3)
          << "the dash_attack at index " << i << " did not follow a dash";
    }
  }
}

// A dash at the very end of the run has no follow-up yet, so the only allowed
// difference between the two counts is that last unfinished pair.
TEST(GoblinTest, LevelThreeEveryDashIsPaidBackExactlyOnce) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

  const int dashes = CountOf(moves, EnemySkill::kGoblinDashLv3);
  const int attacks = CountOf(moves, EnemySkill::kGoblinDashAttackLv3);
  const bool ends_on_a_dash = moves.back() == EnemySkill::kGoblinDashLv3;

  EXPECT_GT(dashes, 0);
  EXPECT_EQ(attacks, dashes - (ends_on_a_dash ? 1 : 0));
}

// A dash costs a turn and then hits back, so it eats two of the thousand
// calls. Only the rolls that were actually rolled can be checked against
// 30 / 40 / 30.
TEST(GoblinTest, LevelThreeRollsAreAboutThirtyFortyThirty) {
  Goblin goblin(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);
  const std::vector<EnemySkill> rolls = RollsOnly(moves);
  const int total = static_cast<int>(rolls.size());

  EXPECT_NEAR(CountOf(rolls, EnemySkill::kGoblinHitLv3), total * 30 / 100,
              kTolerance);
  EXPECT_NEAR(CountOf(rolls, EnemySkill::kGoblinHardSlamLv3), total * 40 / 100,
              kTolerance);
  EXPECT_NEAR(CountOf(rolls, EnemySkill::kGoblinDashLv3), total * 30 / 100,
              kTolerance);
}

TEST(GoblinTest, ADefeatedGoblinReturnsAlreadyDead) {
  Goblin goblin(2, kSeed);
  goblin.TakeDamage(40);  // Level 2 has exactly 40 hp.

  ASSERT_FALSE(goblin.IsAlive());
  EXPECT_EQ(goblin.CalculateMove(), EnemySkill::kAlreadyDead);
}

// The battle code may still ask a corpse for a move once; it must keep saying
// "already dead" instead of rolling again.
TEST(GoblinTest, ADefeatedGoblinKeepsReturningAlreadyDead) {
  Goblin goblin(3, kSeed);
  goblin.TakeDamage(500);

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(goblin.CalculateMove(), EnemySkill::kAlreadyDead);
  }
}

TEST(GoblinTest, ALivingGoblinNeverReturnsAlreadyDead) {
  for (int level = 1; level <= 3; ++level) {
    Goblin goblin(level, kSeed);
    const std::vector<EnemySkill> moves = RollMoves(goblin, kRolls);

    EXPECT_EQ(CountOf(moves, EnemySkill::kAlreadyDead), 0) << level;
  }
}
