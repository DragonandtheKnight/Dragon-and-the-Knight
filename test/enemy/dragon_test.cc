// Tests for Dragon, the final boss.
//
// Two rules shape the fight. Flying is only allowed while the dragon is still
// above half health, and a dragon that flew must slam on the next turn - the
// same forced follow-up a goblin has with its dash. Roar is the opposite: it
// is the one heal the boss has, it only happens once in a fight, and it needs
// the dragon to be badly hurt first. How often any of this fires is balance
// and stays in the implementation.

#include "enemy/Dragon.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

#include "gtest/gtest.h"

namespace {

constexpr int kRolls = 200;

constexpr std::uint32_t kSeed = 20261108;

std::vector<Enemy_skill> RollMoves(Dragon& dragon, int count) {
  std::vector<Enemy_skill> moves;
  moves.reserve(count);
  for (int i = 0; i < count; ++i) {
    moves.push_back(dragon.calMove());
  }
  return moves;
}

int CountOf(const std::vector<Enemy_skill>& moves, Enemy_skill skill) {
  return static_cast<int>(std::count(moves.begin(), moves.end(), skill));
}

// Every skill the dragon of this level is allowed to pick, and no other.
std::set<Enemy_skill> SkillsOf(int level) {
  switch (level) {
    case 1:
      return {Enemy_skill::Dragon_claw_lv1, Enemy_skill::Dragon_breath_lv1};
    case 2:
      return {Enemy_skill::Dragon_claw_lv2, Enemy_skill::Dragon_breath_lv2,
              Enemy_skill::Dragon_fly_lv2, Enemy_skill::Dragon_drop_slam_lv2};
    default:
      return {Enemy_skill::Dragon_claw_lv3, Enemy_skill::Dragon_breath_lv3,
              Enemy_skill::Dragon_fly_lv3,
              Enemy_skill::Dragon_drop_slam_lv3,
              Enemy_skill::Dragon_roar_lv3};
  }
}

// Leaves the dragon at the given fraction of its health, rounded down.
void HurtTo(Dragon& dragon, int numerator, int denominator) {
  dragon.deal_dmg(dragon.get_max_hp() -
                  dragon.get_max_hp() * numerator / denominator);
}

}  // namespace

// Checked at full health and again when badly hurt, so a skill that only
// belongs to one of those moments cannot sneak in through the other.
TEST(DragonTest, DragonOnlyUsesTheSkillsOfItsOwnLevel) {
  for (int level = 1; level <= 3; ++level) {
    const std::set<Enemy_skill> allowed = SkillsOf(level);

    Dragon healthy(level, kSeed);
    const std::vector<Enemy_skill> full = RollMoves(healthy, kRolls);
    for (std::size_t i = 0; i < full.size(); ++i) {
      EXPECT_EQ(allowed.count(full[i]), 1u)
          << "level " << level << ", full health, index " << i;
    }

    Dragon hurt(level, kSeed);
    HurtTo(hurt, 1, 5);
    const std::vector<Enemy_skill> low = RollMoves(hurt, kRolls);
    for (std::size_t i = 0; i < low.size(); ++i) {
      EXPECT_EQ(allowed.count(low[i]), 1u)
          << "level " << level << ", hurt, index " << i;
    }
  }
}

TEST(DragonTest, HealthyDragonNeverRoars) {
  Dragon dragon(3, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls);

  EXPECT_EQ(CountOf(moves, Enemy_skill::Dragon_roar_lv3), 0);
}

TEST(DragonTest, DragonSlightlyHurtNeverRoars) {
  Dragon dragon(3, kSeed);
  HurtTo(dragon, 3, 5);
  const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls);

  EXPECT_EQ(CountOf(moves, Enemy_skill::Dragon_roar_lv3), 0);
}

// Flying is only allowed while the dragon still has more than half its health.
TEST(DragonTest, DragonBelowHalfHealthNeverFlies) {
  for (int level = 2; level <= 3; ++level) {
    Dragon dragon(level, kSeed);
    HurtTo(dragon, 2, 5);
    const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls);

    EXPECT_EQ(CountOf(moves, Enemy_skill::Dragon_fly_lv2), 0) << level;
    EXPECT_EQ(CountOf(moves, Enemy_skill::Dragon_fly_lv3), 0) << level;
  }
}

TEST(DragonTest, FlyingDragonAlwaysSlamsOnTheNextTurn) {
  const std::set<std::pair<int, Enemy_skill>> flights = {
      {2, Enemy_skill::Dragon_fly_lv2}, {3, Enemy_skill::Dragon_fly_lv3}};
  (void)flights;

  Dragon two(2, kSeed);
  const std::vector<Enemy_skill> moves = RollMoves(two, kRolls);

  int flights_seen = 0;
  for (std::size_t i = 0; i + 1 < moves.size(); ++i) {
    if (moves[i] == Enemy_skill::Dragon_fly_lv2) {
      ++flights_seen;
      EXPECT_EQ(moves[i + 1], Enemy_skill::Dragon_drop_slam_lv2)
          << "the flight at index " << i << " did not slam";
    }
  }

  EXPECT_GT(flights_seen, 0);
}

TEST(DragonTest, SlamsOnlyHappenAfterAFlight) {
  for (int level = 2; level <= 3; ++level) {
    Dragon dragon(level, kSeed);
    const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls);
    const Enemy_skill slam = level == 2 ? Enemy_skill::Dragon_drop_slam_lv2
                                        : Enemy_skill::Dragon_drop_slam_lv3;

    for (std::size_t i = 0; i < moves.size(); ++i) {
      if (moves[i] != slam) {
        continue;
      }
      EXPECT_GT(i, 0) << "level " << level << " slammed on its first turn";
      if (i == 0) {
        continue;
      }
      const bool came_from_flight =
          moves[i - 1] == (level == 2 ? Enemy_skill::Dragon_fly_lv2
                                      : Enemy_skill::Dragon_fly_lv3);
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
  const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls * 3);

  EXPECT_LE(CountOf(moves, Enemy_skill::Dragon_roar_lv3), 1);
}

TEST(DragonTest, DyingDragonRoarsWhenItIsNearlyOut) {
  Dragon dragon(3, kSeed);
  HurtTo(dragon, 1, 5);
  const std::vector<Enemy_skill> moves = RollMoves(dragon, kRolls);

  EXPECT_GT(CountOf(moves, Enemy_skill::Dragon_roar_lv3), 0);
}

TEST(DragonTest, DeadDragonReturnsAlreadyDead) {
  Dragon dragon(1, kSeed);
  dragon.deal_dmg(dragon.get_hp());

  ASSERT_FALSE(dragon.is_alive());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(dragon.calMove(), Enemy_skill::Already_dead);
  }
}
