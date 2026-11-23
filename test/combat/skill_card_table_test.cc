#include "combat/combat.h"

#include <cstdint>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
#include "enemy/slime.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;

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
  Player player("Knight", 100, kSeed);
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
  Player player("Knight", 100, kSeed);
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

// A skill that has no card in the table must resolve to nothing.
TEST(SkillCardTableTest, ASkillWithoutACardDoesNothing) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 100);
}
