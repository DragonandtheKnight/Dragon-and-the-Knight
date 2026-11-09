// Tests for Giant, the mini boss that winds up before it swings.
//
// Level 3 has the one rule that matters: the super charge is always followed
// by the super charge hit on the next turn, the same way a goblin's dash is
// paid back by a dash attack. Everything else here checks that a giant only
// picks from its own level's skill list; how likely each pick is stays in the
// implementation.

#include "enemy/Giant.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 200;

constexpr std::uint32_t kSeed = 20261108;

std::vector<Enemy_skill> RollMoves(Giant& giant, int count) {
  std::vector<Enemy_skill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(giant.calMove());
  }
  return moves;
}

int CountOf(const std::vector<Enemy_skill>& moves, Enemy_skill skill) {
  return static_cast<int>(std::count(moves.begin(), moves.end(), skill));
}

// Every skill the giant of this level is allowed to pick, and no other.
std::set<Enemy_skill> SkillsOf(int level) {
  switch (level) {
    case 1:
      return {Enemy_skill::Giant_slam_lv1, Enemy_skill::Giant_defend_lv1};
    case 2:
      return {Enemy_skill::Giant_slam_lv2, Enemy_skill::Giant_defend_lv2,
              Enemy_skill::Giant_charge_lv2};
    default:
      return {Enemy_skill::Giant_slam_lv3, Enemy_skill::Giant_defend_lv3,
              Enemy_skill::Giant_super_charge_lv3,
              Enemy_skill::Giant_super_charge_hit_lv3};
  }
}

}  // namespace

TEST(GiantTest, GiantOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    Giant giant(level, kSeed);
    const std::vector<Enemy_skill> moves = RollMoves(giant, kRolls);
    const std::set<Enemy_skill> allowed = SkillsOf(level);

    for (std::size_t i = 0; i < moves.size(); ++i) {
      EXPECT_EQ(allowed.count(moves[i]), 1u)
          << "level " << level << ", index " << i;
    }
  }
}

TEST(GiantTest, LevelThreeChargeIsAlwaysFollowedByTheChargeHit) {
  Giant giant(3, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(giant, kRolls);

  int charges = 0;
  for (std::size_t i = 0; i + 1 < moves.size(); ++i) {
    if (moves[i] == Enemy_skill::Giant_super_charge_lv3) {
      ++charges;
      EXPECT_EQ(moves[i + 1], Enemy_skill::Giant_super_charge_hit_lv3)
          << "the charge at index " << i << " was not paid back";
    }
  }

  EXPECT_GT(charges, 0);
}

TEST(GiantTest, LevelThreeNeverOpensWithAChargeHit) {
  Giant giant(3, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(giant, kRolls);

  ASSERT_FALSE(moves.empty());
  EXPECT_NE(moves.front(), Enemy_skill::Giant_super_charge_hit_lv3);
}

TEST(GiantTest, LevelThreeChargeHitNeverComesFromAnywhereElse) {
  Giant giant(3, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(giant, kRolls);

  for (std::size_t i = 1; i < moves.size(); ++i) {
    if (moves[i] == Enemy_skill::Giant_super_charge_hit_lv3) {
      EXPECT_EQ(moves[i - 1], Enemy_skill::Giant_super_charge_lv3)
          << "the charge hit at index " << i << " did not follow a charge";
    }
  }
}

// A charge at the very end of the run has no follow-up yet, so the only
// allowed difference between the two counts is that last unfinished pair.
TEST(GiantTest, LevelThreeEveryChargeIsPaidBackExactlyOnce) {
  Giant giant(3, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(giant, kRolls);

  const int charges = CountOf(moves, Enemy_skill::Giant_super_charge_lv3);
  const int hits = CountOf(moves, Enemy_skill::Giant_super_charge_hit_lv3);
  const bool ends_on_a_charge =
      moves.back() == Enemy_skill::Giant_super_charge_lv3;

  EXPECT_GT(charges, 0);
  EXPECT_EQ(hits, charges - (ends_on_a_charge ? 1 : 0));
}

TEST(GiantTest, DeadGiantReturnsAlreadyDead) {
  Giant giant(1, kSeed);
  giant.deal_dmg(giant.get_hp());

  ASSERT_FALSE(giant.is_alive());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(giant.calMove(), Enemy_skill::Already_dead);
  }
}
