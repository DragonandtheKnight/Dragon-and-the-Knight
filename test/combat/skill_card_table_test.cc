#include "combat/combat.h"

#include <cstdint>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
#include "enemy/shaman.h"
#include "enemy/slime.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;
constexpr int kPlayerMaxHp = 100;
constexpr std::uint32_t kSeedCount = 60;

Card MakeCard() {
  return Card("Test card", "A card for tests", static_cast<CardRarity>(0),
              static_cast<CardType>(0), 1, 5);
}

void FillDeck(Player& player, int count) {
  for (int i = 0; i < count; ++i) {
    player.GetDeck().AddCard(MakeCard());
  }
}
}  // namespace

// The level 1 slime always slaps while it is at full health, so this damage is
// deterministic and does not depend on the seed.
TEST(SkillCardTableTest, SlimeSlapLv1DamagesThePlayer) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Slime slime(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&slime, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 97);
}

// Every living slot casts its own skill, so four slimes deal four slaps.
TEST(SkillCardTableTest, EverySlimeSlotCastsItsOwnSkill) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Slime first(1, kSeed);
  Slime second(1, kSeed);
  Slime third(1, kSeed);
  Slime fourth(1, kSeed);

  std::vector<Enemy*> slots = {&first, &second, &third, &fourth};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 88);
}

// A skill that has no card in the table must resolve to nothing. Neither level
// 1 shaman skill has a card, so neither one may hurt the player.
TEST(SkillCardTableTest, ASkillWithoutACardDoesNothing) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Shaman shaman(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&shaman, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), kPlayerMaxHp);
}

// The level 2 slime slaps for 5 while it is at full health.
TEST(SkillCardTableTest, SlimeSlapLv2DamagesThePlayer) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Slime slime(2, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&slime, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 95);
}

// The level 3 slime slaps for 8 while it is at full health.
TEST(SkillCardTableTest, SlimeSlapLv3DamagesThePlayer) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Slime slime(3, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&slime, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 92);
}

// A level 1 goblin always hits for 5.
TEST(SkillCardTableTest, GoblinHitLv1DamagesThePlayer) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 95);
}

// A damaged level 2 slime either slaps for 5 or heals itself for 8, so one
// round always ends in one of exactly two states. Both outcomes have to show
// up, so the test walks over many seeds instead of pinning one roll: the rolls
// of uniform_int_distribution are not the same on every platform.
TEST(SkillCardTableTest, SlimeLv2SlapsOrHealsForEight) {
  int slap_seen = 0;
  int heal_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Slime slime(2, seed);
    slime.TakeDamage(10);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&slime, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 95 && slime.GetHp() == 20) {
      ++slap_seen;
    } else if (player.GetHp() == kPlayerMaxHp && slime.GetHp() == 28) {
      ++heal_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(slap_seen, 0);
  EXPECT_GT(heal_seen, 0);
}

// The level 3 slime slaps for 8 or heals itself for 12.
TEST(SkillCardTableTest, SlimeLv3SlapsOrHealsForTwelve) {
  int slap_seen = 0;
  int heal_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Slime slime(3, seed);
    slime.TakeDamage(20);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&slime, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 92 && slime.GetHp() == 20) {
      ++slap_seen;
    } else if (player.GetHp() == kPlayerMaxHp && slime.GetHp() == 32) {
      ++heal_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(slap_seen, 0);
  EXPECT_GT(heal_seen, 0);
}

// The level 2 goblin hits for 8 or hard slams for 12.
TEST(SkillCardTableTest, GoblinLv2HitsForEightOrHardSlamsForTwelve) {
  int hit_seen = 0;
  int slam_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Goblin goblin(2, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 92) {
      ++hit_seen;
    } else if (player.GetHp() == 88) {
      ++slam_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(hit_seen, 0);
  EXPECT_GT(slam_seen, 0);
}

// The level 3 goblin hits for 12, hard slams for 18, or dashes behind a 15
// shield instead of attacking at all.
TEST(SkillCardTableTest, GoblinLv3HitsHardSlamsOrDashesBehindAShield) {
  int hit_seen = 0;
  int slam_seen = 0;
  int dash_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Goblin goblin(3, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 88) {
      ++hit_seen;
    } else if (player.GetHp() == 82) {
      ++slam_seen;
    } else if (player.GetHp() == kPlayerMaxHp && goblin.GetShield() == 15) {
      ++dash_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(hit_seen, 0);
  EXPECT_GT(slam_seen, 0);
  EXPECT_GT(dash_seen, 0);
}
