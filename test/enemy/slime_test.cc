// Tests for Slime, the small monster that patches itself up.
//
// The rule under test is the one behind calMove(): a slime only heals while it
// is hurt, and a slime at full health always attacks. How often it heals and
// how much it heals for is balance, left to the implementation - no
// percentage and no number of hp is pinned here.

#include "enemy/Slime.h"

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
std::vector<Enemy_skill> RollMoves(Slime& slime, int count) {
  std::vector<Enemy_skill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(slime.calMove());
  }
  return moves;
}

bool Contains(const std::vector<Enemy_skill>& moves, Enemy_skill skill) {
  return std::find(moves.begin(), moves.end(), skill) != moves.end();
}

Enemy_skill SlapOf(int level) {
  switch (level) {
    case 1:
      return Enemy_skill::Slime_slap_lv1;
    case 2:
      return Enemy_skill::Slime_slap_lv2;
    default:
      return Enemy_skill::Slime_slap_lv3;
  }
}

Enemy_skill HealOf(int level) {
  switch (level) {
    case 1:
      return Enemy_skill::Slime_heal_lv1;
    case 2:
      return Enemy_skill::Slime_heal_lv2;
    default:
      return Enemy_skill::Slime_heal_lv3;
  }
}

// Leaves the slime at exactly one hp, whatever its maximum turns out to be.
void LeaveOneHp(Slime& slime) { slime.deal_dmg(slime.get_hp() - 1); }

}  // namespace

// A slime that has not been hit has nothing to heal, so it can only attack.
TEST(SlimeTest, FullHealthSlimeOnlyAttacks) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    const std::vector<Enemy_skill> moves = RollMoves(slime, kRolls);

    EXPECT_EQ(static_cast<int>(std::count(moves.begin(), moves.end(),
                                          SlapOf(level))),
              kRolls)
        << "level " << level;
  }
}

TEST(SlimeTest, WoundedSlimeUsesItsHealSkill) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<Enemy_skill> moves = RollMoves(slime, kRolls);

    EXPECT_TRUE(Contains(moves, HealOf(level))) << "level " << level;
  }
}

// Healing is on top of attacking, not instead of it: a hurt slime still has
// to hurt back.
TEST(SlimeTest, WoundedSlimeStillAttacks) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<Enemy_skill> moves = RollMoves(slime, kRolls);

    EXPECT_TRUE(Contains(moves, SlapOf(level))) << "level " << level;
  }
}

TEST(SlimeTest, SlimeOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    Slime slime(level, kSeed);
    LeaveOneHp(slime);
    const std::vector<Enemy_skill> moves = RollMoves(slime, kRolls);

    for (std::size_t i = 0; i < moves.size(); ++i) {
      const bool own = moves[i] == SlapOf(level) || moves[i] == HealOf(level);
      EXPECT_TRUE(own) << "level " << level << ", index " << i;
    }
  }
}

TEST(SlimeTest, DeadSlimeReturnsAlreadyDead) {
  Slime slime(1, kSeed);
  slime.deal_dmg(slime.get_hp());

  ASSERT_FALSE(slime.is_alive());
  EXPECT_EQ(slime.calMove(), Enemy_skill::Already_dead);
}

TEST(SlimeTest, DeadSlimeKeepsReturningAlreadyDead) {
  Slime slime(2, kSeed);
  slime.deal_dmg(slime.get_hp());

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(slime.calMove(), Enemy_skill::Already_dead);
  }
}
