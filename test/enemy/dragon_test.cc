// Tests for Dragon, the final boss.
//
// Two rules shape the fight. Flying is only allowed while the dragon is still
// above half health, and a dragon that flew must slam on the next turn - the
// same forced follow-up a goblin has with its dash. Roar is the opposite: it
// is the one heal the boss has, it only happens once in a fight, and it needs
// the dragon to be badly hurt first. How often any of this fires is balance
// and stays in the implementation.

#include "enemy/dragon.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 200;

constexpr std::uint32_t kSeed = 20261108;

std::vector<EnemySkill> RollMoves(Dragon& dragon, int count) {
  std::vector<EnemySkill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(dragon.CalculateMove());
  }
  return moves;
}

int CountOf(const std::vector<EnemySkill>& moves, EnemySkill skill) {
  return static_cast<int>(std::count(moves.begin(), moves.end(), skill));
}

// Every skill the dragon of this level is allowed to pick, and no other.
std::set<EnemySkill> SkillsOf(int level) {
  switch (level) {
    case 1:
      return {EnemySkill::kDragonClawLv1, EnemySkill::kDragonBreathLv1};
    case 2:
      return {EnemySkill::kDragonClawLv2, EnemySkill::kDragonBreathLv2,
              EnemySkill::kDragonFlyLv2, EnemySkill::kDragonDropSlamLv2};
    default:
      return {EnemySkill::kDragonClawLv3, EnemySkill::kDragonBreathLv3,
              EnemySkill::kDragonFlyLv3, EnemySkill::kDragonDropSlamLv3,
              EnemySkill::kDragonRoarLv3};
  }
}

// Leaves the dragon at the given fraction of its health, rounded down.
void HurtTo(Dragon& dragon, int numerator, int denominator) {
  dragon.TakeDamage(dragon.GetMaxHp() -
                    dragon.GetMaxHp() * numerator / denominator);
}

}  // namespace

// Checked at full health and again when badly hurt, so a skill that only
// belongs to one of those moments cannot sneak in through the other.
TEST(DragonTest, DragonOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    const std::set<EnemySkill> allowed = SkillsOf(level);

    Dragon healthy(level, kSeed);
    const std::vector<EnemySkill> full = RollMoves(healthy, kRolls);
    for (std::size_t i = 0; i < full.size(); ++i) {
      EXPECT_EQ(allowed.count(full[i]), 1u)
          << "level " << level << ", full health, index " << i;
    }

    Dragon hurt(level, kSeed);
    HurtTo(hurt, 1, 5);
    const std::vector<EnemySkill> low = RollMoves(hurt, kRolls);
    for (std::size_t i = 0; i < low.size(); ++i) {
      EXPECT_EQ(allowed.count(low[i]), 1u)
          << "level " << level << ", hurt, index " << i;
    }
  }
}

TEST(DragonTest, HealthyDragonNeverRoars) {
  Dragon dragon(3, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls);

  EXPECT_EQ(CountOf(moves, EnemySkill::kDragonRoarLv3), 0);
}

TEST(DragonTest, DragonSlightlyHurtNeverRoars) {
  Dragon dragon(3, kSeed);
  HurtTo(dragon, 3, 5);
  const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls);

  EXPECT_EQ(CountOf(moves, EnemySkill::kDragonRoarLv3), 0);
}

// Flying is only allowed while the dragon still has more than half its health.
TEST(DragonTest, DragonBelowHalfHealthNeverFlies) {
  for (int level = 2; level <= 3; ++level) {
    Dragon dragon(level, kSeed);
    HurtTo(dragon, 2, 5);
    const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls);

    EXPECT_EQ(CountOf(moves, EnemySkill::kDragonFlyLv2), 0) << level;
    EXPECT_EQ(CountOf(moves, EnemySkill::kDragonFlyLv3), 0) << level;
  }
}

TEST(DragonTest, FlyingDragonAlwaysSlamsOnTheNextTurn) {
  const std::set<std::pair<int, EnemySkill>> flights = {
      {2, EnemySkill::kDragonFlyLv2}, {3, EnemySkill::kDragonFlyLv3}};
  (void)flights;

  Dragon two(2, kSeed);
  const std::vector<EnemySkill> moves = RollMoves(two, kRolls);

  int flights_seen = 0;
  for (std::size_t i = 0; i + 1 < moves.size(); ++i) {
    if (moves[i] == EnemySkill::kDragonFlyLv2) {
      ++flights_seen;
      EXPECT_EQ(moves[i + 1], EnemySkill::kDragonDropSlamLv2)
          << "the flight at index " << i << " did not slam";
    }
  }

  EXPECT_GT(flights_seen, 0);
}

TEST(DragonTest, SlamsOnlyHappenAfterAFlight) {
  for (int level = 2; level <= 3; ++level) {
    Dragon dragon(level, kSeed);
    const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls);
    const EnemySkill slam = level == 2 ? EnemySkill::kDragonDropSlamLv2
                                       : EnemySkill::kDragonDropSlamLv3;

    for (std::size_t i = 0; i < moves.size(); ++i) {
      if (moves[i] != slam) {
        continue;
      }
      EXPECT_GT(i, 0) << "level " << level << " slammed on its first turn";
      if (i == 0) {
        continue;
      }
      const bool came_from_flight =
          moves[i - 1] ==
          (level == 2 ? EnemySkill::kDragonFlyLv2 : EnemySkill::kDragonFlyLv3);
      EXPECT_TRUE(came_from_flight)
          << level << " slammed without flying at " << i;
    }
  }
}

// The whole point of roar is that it happens once: a fight where it can fire
// twice is a fight the player cannot win by planning.
TEST(DragonTest, RoarHappensAtMostOnceEvenWhenBadlyHurt) {
  Dragon dragon(3, kSeed);
  HurtTo(dragon, 1, 5);
  const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls * 3);

  EXPECT_LE(CountOf(moves, EnemySkill::kDragonRoarLv3), 1);
}

TEST(DragonTest, DyingDragonRoarsWhenItIsNearlyOut) {
  Dragon dragon(3, kSeed);
  HurtTo(dragon, 1, 5);
  const std::vector<EnemySkill> moves = RollMoves(dragon, kRolls);

  EXPECT_GT(CountOf(moves, EnemySkill::kDragonRoarLv3), 0);
}

TEST(DragonTest, DeadDragonReturnsAlreadyDead) {
  Dragon dragon(1, kSeed);
  dragon.TakeDamage(dragon.GetHp());

  ASSERT_FALSE(dragon.IsAlive());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(dragon.CalculateMove(), EnemySkill::kAlreadyDead);
  }
}
