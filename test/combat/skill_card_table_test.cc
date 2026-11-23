#include <cstdint>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "combat/combat.h"
#include "enemy/dragon.h"
#include "enemy/empty_enemy.h"
#include "enemy/giant.h"
#include "enemy/goblin.h"
#include "enemy/shaman.h"
#include "enemy/slime.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;
constexpr int kPlayerMaxHp = 100;
constexpr std::uint32_t kSeedCount = 100;

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

// The level 1 shaman either curses the player for 5 or heals every enemy for
// 10% of its own max health. A shaman already hurt by 10 shows both outcomes.
TEST(SkillCardTableTest, ShamanLv1CursesOrHealsEveryEnemy) {
  int curse_seen = 0;
  int heal_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Shaman shaman(1, seed);
    shaman.TakeDamage(10);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&shaman, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 95 && shaman.GetHp() == 30) {
      ++curse_seen;
    } else if (player.GetHp() == kPlayerMaxHp && shaman.GetHp() == 34) {
      ++heal_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(curse_seen, 0);
  EXPECT_GT(heal_seen, 0);
}

// The level 2 shaman curses for 8, heals every enemy for 15% of its max
// health, or gives the healthiest enemy a 12 shield.
TEST(SkillCardTableTest, ShamanLv2CursesHealsOrShieldsTheHealthiestEnemy) {
  int curse_seen = 0;
  int heal_seen = 0;
  int shield_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Shaman shaman(2, seed);
    shaman.TakeDamage(10);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&shaman, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 92 && shaman.GetHp() == 45) {
      ++curse_seen;
    } else if (player.GetHp() == kPlayerMaxHp && shaman.GetHp() == 53) {
      ++heal_seen;
    } else if (player.GetHp() == kPlayerMaxHp && shaman.GetHp() == 45 &&
               shaman.GetShield() == 12) {
      ++shield_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(curse_seen, 0);
  EXPECT_GT(heal_seen, 0);
  EXPECT_GT(shield_seen, 0);
}

// At half health or below the level 3 shaman always heals itself for 20
// instead of rolling, so this one is deterministic.
TEST(SkillCardTableTest, ShamanLv3HealsItselfBelowHalfHealth) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Shaman shaman(3, kSeed);
  shaman.TakeDamage(35);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&shaman, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), kPlayerMaxHp);
  EXPECT_EQ(shaman.GetHp(), 55);
}

// Above half health the level 3 shaman curses for 12, heals every enemy for
// 20% of its max health, or gives the healthiest enemy a 16 shield.
TEST(SkillCardTableTest, ShamanLv3CursesHealsOrShieldsTheHealthiestEnemy) {
  int curse_seen = 0;
  int heal_seen = 0;
  int shield_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Shaman shaman(3, seed);
    shaman.TakeDamage(10);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&shaman, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 88 && shaman.GetHp() == 60) {
      ++curse_seen;
    } else if (player.GetHp() == kPlayerMaxHp && shaman.GetHp() == 70) {
      ++heal_seen;
    } else if (player.GetHp() == kPlayerMaxHp && shaman.GetHp() == 60 &&
               shaman.GetShield() == 16) {
      ++shield_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(curse_seen, 0);
  EXPECT_GT(heal_seen, 0);
  EXPECT_GT(shield_seen, 0);
}

// The level 1 giant either slams the player for 8 or raises a 12 shield.
TEST(SkillCardTableTest, GiantLv1SlamsOrDefends) {
  int slam_seen = 0;
  int defend_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Giant giant(1, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&giant, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 92 && giant.GetShield() == 0) {
      ++slam_seen;
    } else if (player.GetHp() == kPlayerMaxHp && giant.GetShield() == 12) {
      ++defend_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(slam_seen, 0);
  EXPECT_GT(defend_seen, 0);
}

// The level 2 giant slams for 12, raises a 16 shield, or lands a charge for 16.
TEST(SkillCardTableTest, GiantLv2SlamsDefendsOrCharges) {
  int slam_seen = 0;
  int defend_seen = 0;
  int charge_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Giant giant(2, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&giant, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 88) {
      ++slam_seen;
    } else if (player.GetHp() == kPlayerMaxHp && giant.GetShield() == 16) {
      ++defend_seen;
    } else if (player.GetHp() == 84) {
      ++charge_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(slam_seen, 0);
  EXPECT_GT(defend_seen, 0);
  EXPECT_GT(charge_seen, 0);
}

// The level 3 giant slams for 16, raises a 20 shield, or spends the whole
// round charging and deals no damage at all.
TEST(SkillCardTableTest, GiantLv3SlamsDefendsOrSuperCharges) {
  int slam_seen = 0;
  int defend_seen = 0;
  int charge_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Giant giant(3, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&giant, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 84) {
      ++slam_seen;
    } else if (player.GetHp() == kPlayerMaxHp && giant.GetShield() == 20) {
      ++defend_seen;
    } else if (player.GetHp() == kPlayerMaxHp && giant.GetShield() == 0) {
      ++charge_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(slam_seen, 0);
  EXPECT_GT(defend_seen, 0);
  EXPECT_GT(charge_seen, 0);
}

// The charge costs the giant a turn, and the blow that follows deals 34.
TEST(SkillCardTableTest, GiantSuperChargeHitsForThirtyFour) {
  bool hit_seen = false;
  for (std::uint32_t seed = 1; seed <= kSeedCount && !hit_seen; ++seed) {
    Player player("Knight", 1000, seed);
    Giant giant(3, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&giant, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 100);
    combat.StartBattle();

    for (int round = 0; round < 10; ++round) {
      if (combat.GetNextAct(0) == EnemySkill::kGiantSuperChargeHitLv3) {
        int before = player.GetHp();
        combat.EndRound();
        EXPECT_EQ(before - player.GetHp(), 34);
        hit_seen = true;
        break;
      }
      combat.EndRound();
    }
  }
  EXPECT_TRUE(hit_seen);
}

// The level 1 dragon either claws for 10 or breathes for 14.
TEST(SkillCardTableTest, DragonLv1ClawsOrBreathes) {
  int claw_seen = 0;
  int breath_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Dragon dragon(1, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 90) {
      ++claw_seen;
    } else if (player.GetHp() == 86) {
      ++breath_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(claw_seen, 0);
  EXPECT_GT(breath_seen, 0);
}

// At full health the level 2 dragon claws for 14, breathes for 18, or flies up
// behind a 999 shield instead of attacking.
TEST(SkillCardTableTest, DragonLv2ClawsBreathesOrFlies) {
  int claw_seen = 0;
  int breath_seen = 0;
  int fly_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Dragon dragon(2, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 86 && dragon.GetShield() == 0) {
      ++claw_seen;
    } else if (player.GetHp() == 82 && dragon.GetShield() == 0) {
      ++breath_seen;
    } else if (player.GetHp() == kPlayerMaxHp && dragon.GetShield() == 999) {
      ++fly_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(claw_seen, 0);
  EXPECT_GT(breath_seen, 0);
  EXPECT_GT(fly_seen, 0);
}

// The dive that follows a flight deals 25 to the level 2 dragon's target.
TEST(SkillCardTableTest, DragonLv2DropSlamHitsForTwentyFive) {
  bool hit_seen = false;
  for (std::uint32_t seed = 1; seed <= kSeedCount && !hit_seen; ++seed) {
    Player player("Knight", 1000, seed);
    Dragon dragon(2, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 100);
    combat.StartBattle();

    for (int round = 0; round < 10; ++round) {
      if (combat.GetNextAct(0) == EnemySkill::kDragonDropSlamLv2) {
        int before = player.GetHp();
        combat.EndRound();
        EXPECT_EQ(before - player.GetHp(), 25);
        hit_seen = true;
        break;
      }
      combat.EndRound();
    }
  }
  EXPECT_TRUE(hit_seen);
}

// At full health the level 3 dragon claws for 18, breathes for 24, or flies up
// behind a 999 shield instead of attacking.
TEST(SkillCardTableTest, DragonLv3ClawsBreathesOrFlies) {
  int claw_seen = 0;
  int breath_seen = 0;
  int fly_seen = 0;
  int unexpected = 0;
  for (std::uint32_t seed = 1; seed <= kSeedCount; ++seed) {
    Player player("Knight", kPlayerMaxHp, seed);
    Dragon dragon(3, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 20);
    combat.StartBattle();

    combat.EndRound();

    if (player.GetHp() == 82 && dragon.GetShield() == 0) {
      ++claw_seen;
    } else if (player.GetHp() == 76 && dragon.GetShield() == 0) {
      ++breath_seen;
    } else if (player.GetHp() == kPlayerMaxHp && dragon.GetShield() == 999) {
      ++fly_seen;
    } else {
      ++unexpected;
    }
  }
  EXPECT_EQ(unexpected, 0);
  EXPECT_GT(claw_seen, 0);
  EXPECT_GT(breath_seen, 0);
  EXPECT_GT(fly_seen, 0);
}

// The dive that follows a flight deals 35 to the level 3 dragon's target.
TEST(SkillCardTableTest, DragonLv3DropSlamHitsForThirtyFive) {
  bool hit_seen = false;
  for (std::uint32_t seed = 1; seed <= kSeedCount && !hit_seen; ++seed) {
    Player player("Knight", 1000, seed);
    Dragon dragon(3, seed);
    EmptyEnemy empty(0, seed);

    std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
    Combat combat(player, slots);
    FillDeck(player, 100);
    combat.StartBattle();

    for (int round = 0; round < 10; ++round) {
      if (combat.GetNextAct(0) == EnemySkill::kDragonDropSlamLv3) {
        int before = player.GetHp();
        combat.EndRound();
        EXPECT_EQ(before - player.GetHp(), 35);
        hit_seen = true;
        break;
      }
      combat.EndRound();
    }
  }
  EXPECT_TRUE(hit_seen);
}

// Below a quarter of its health the level 3 dragon always roars, and the roar
// heals it for half of its maximum health. The roar is only available once, so
// the round after it must be an attack or a flight again.
TEST(SkillCardTableTest, DragonLv3RoarsOnceBelowAQuarterOfItsHealth) {
  Player player("Knight", kPlayerMaxHp, kSeed);
  Dragon dragon(3, kSeed);
  dragon.TakeDamage(120);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&dragon, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 40);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), kPlayerMaxHp);
  EXPECT_EQ(dragon.GetHp(), 105);

  int hp_before_second_round = player.GetHp();
  combat.EndRound();
  int taken = hp_before_second_round - player.GetHp();
  EXPECT_TRUE(taken == 18 || taken == 24 ||
              (taken == 0 && dragon.GetShield() == 999));
}
