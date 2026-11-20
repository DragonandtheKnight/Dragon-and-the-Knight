// Tests for Shaman, the caster that keeps the other monsters alive.
//
// Almost everything a shaman does lands on someone else - the whole group, or
// one ally - so the skill name it returns is all Enemy can be asked about
// here; applying the effect is the battle's job. The one rule that is about
// the shaman itself is its self heal: only while badly hurt.

#include "enemy/shaman.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 200;

constexpr std::uint32_t kSeed = 20261108;

std::vector<EnemySkill> RollMoves(Shaman& shaman, int count) {
  std::vector<EnemySkill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(shaman.CalculateMove());
  }
  return moves;
}

// Every skill the shaman of this level is allowed to pick, and no other.
std::set<EnemySkill> SkillsOf(int level) {
  switch (level) {
    case 1:
      return {EnemySkill::kShamanCurseLv1, EnemySkill::kShamanHealAllLv1};
    case 2:
      return {EnemySkill::kShamanCurseLv2, EnemySkill::kShamanHealAllLv2,
              EnemySkill::kShamanGiveShieldLv2};
    default:
      return {EnemySkill::kShamanCurseLv3, EnemySkill::kShamanHealAllLv3,
              EnemySkill::kShamanGiveShieldLv3, EnemySkill::kShamanHealSelfLv3};
  }
}

// Leaves the shaman at one hp, whatever its maximum turns out to be.
void LeaveOneHp(Shaman& shaman) { shaman.TakeDamage(shaman.GetHp() - 1); }

}  // namespace

TEST(ShamanTest, FullHealthShamanNeverHealsItself) {
  for (int level = 1; level <= 3; ++level) {
    Shaman shaman(level, kSeed);
    const std::vector<EnemySkill> moves = RollMoves(shaman, kRolls);

    EXPECT_EQ(static_cast<int>(std::count(moves.begin(), moves.end(),
                                          EnemySkill::kShamanHealSelfLv3)),
              0)
        << "level " << level;
  }
}

// Comfortably above half health is still too healthy to patch itself up.
TEST(ShamanTest, ShamanWellAboveHalfHealthNeverHealsItself) {
  Shaman shaman(3, kSeed);
  shaman.TakeDamage(shaman.GetMaxHp() / 4);
  const std::vector<EnemySkill> moves = RollMoves(shaman, kRolls);

  EXPECT_EQ(static_cast<int>(std::count(moves.begin(), moves.end(),
                                        EnemySkill::kShamanHealSelfLv3)),
            0);
}

TEST(ShamanTest, BadlyHurtLevelThreeShamanHealsItself) {
  Shaman shaman(3, kSeed);
  LeaveOneHp(shaman);
  const std::vector<EnemySkill> moves = RollMoves(shaman, kRolls);

  EXPECT_TRUE(std::find(moves.begin(), moves.end(),
                        EnemySkill::kShamanHealSelfLv3) != moves.end());
}

TEST(ShamanTest, ShamanOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    Shaman shaman(level, kSeed);
    LeaveOneHp(shaman);
    const std::vector<EnemySkill> moves = RollMoves(shaman, kRolls);
    const std::set<EnemySkill> allowed = SkillsOf(level);

    for (std::size_t i = 0; i < moves.size(); ++i) {
      EXPECT_EQ(allowed.count(moves[i]), 1u)
          << "level " << level << ", index " << i;
    }
  }
}

TEST(ShamanTest, DeadShamanReturnsAlreadyDead) {
  Shaman shaman(1, kSeed);
  shaman.TakeDamage(shaman.GetHp());

  ASSERT_FALSE(shaman.IsAlive());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(shaman.CalculateMove(), EnemySkill::kAlreadyDead);
  }
}
