// Tests for Slime, the small monster that patches itself up.
//
// The rule under test is the one behind CalculateMove(): a slime only heals
// while it is hurt, and a slime at full health always attacks. How often it
// heals and how much it heals for is balance, left to the implementation - no
// percentage and no number of hp is pinned here.

#include "enemy/slime.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 200;

constexpr std::uint32_t kSeed = 20261108;

// 200 rolls is enough to notice a skill that the implementation does use;
// nothing here depends on how likely it is.
std::vector<EnemySkill> RollMoves(Slime& slime, int count) {
  std::vector<EnemySkill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(slime.CalculateMove());
  }
  return moves;
}

bool Contains(const std::vector<EnemySkill>& moves, EnemySkill skill) {
  return std::find(moves.begin(), moves.end(), skill) != moves.end();
}

EnemySkill SlapOf(int level) {
  switch (level) {
    case 1:
      return EnemySkill::kSlimeSlapLv1;
    case 2:
      return EnemySkill::kSlimeSlapLv2;
    default:
      return EnemySkill::kSlimeSlapLv3;
  }
}

EnemySkill HealOf(int level) {
  switch (level) {
    case 1:
      return EnemySkill::kSlimeHealLv1;
    case 2:
      return EnemySkill::kSlimeHealLv2;
    default:
      return EnemySkill::kSlimeHealLv3;
  }
}

// Leaves the slime at exactly one hp, whatever its maximum turns out to be.
void LeaveOneHp(Slime& slime) { slime.TakeDamage(slime.GetHp() - 1); }

}  // namespace

// A slime that has not been hit has nothing to heal, so it can only attack.
TEST(SlimeTest, FullHealthSlimeOnlyAttacks) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    const std::vector<EnemySkill> moves = RollMoves(slime, kRolls);

    EXPECT_EQ(
        static_cast<int>(std::count(moves.begin(), moves.end(), SlapOf(level))),
        kRolls)
        << "level " << level;
  }
}

TEST(SlimeTest, WoundedSlimeUsesItsHealSkill) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<EnemySkill> moves = RollMoves(slime, kRolls);

    EXPECT_TRUE(Contains(moves, HealOf(level))) << "level " << level;
  }
}

// Healing is on top of attacking, not instead of it: a hurt slime still has
// to hurt back.
TEST(SlimeTest, WoundedSlimeStillAttacks) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<EnemySkill> moves = RollMoves(slime, kRolls);

    EXPECT_TRUE(Contains(moves, SlapOf(level))) << "level " << level;
  }
}

TEST(SlimeTest, SlimeOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<EnemySkill> moves = RollMoves(slime, kRolls);

    for (std::size_t i = 0; i < moves.size(); ++i) {
      const bool own = moves[i] == SlapOf(level) || moves[i] == HealOf(level);
      EXPECT_TRUE(own) << "level " << level << ", index " << i;
    }
  }
}

TEST(SlimeTest, DeadSlimeReturnsAlreadyDead) {
  Slime slime(1, kSeed);
  slime.TakeDamage(slime.GetHp());

  ASSERT_FALSE(slime.IsAlive());
  EXPECT_EQ(slime.CalculateMove(), EnemySkill::kAlreadyDead);
}

TEST(SlimeTest, DeadSlimeKeepsReturningAlreadyDead) {
  Slime slime(2, kSeed);
  slime.TakeDamage(slime.GetHp());

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(slime.CalculateMove(), EnemySkill::kAlreadyDead);
  }
}
