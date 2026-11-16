#include "combat/combat.h"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "enemy/empty_enemy.h"
#include "enemy/enemy_skill.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;

Card MakeCard(int cost = 1) {
  return Card("Test card", "A card for tests", static_cast<CardRarity>(0),
              static_cast<CardType>(0), cost, 5);
}

void FillDeck(Player& player, int count, int cost = 1) {
  for (int i = 0; i < count; ++i) {
    player.GetDeck().AddCard(MakeCard(cost));
  }
}
}  // namespace

TEST(CombatConstructorTest, ExposesSlotsInOrder) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  EXPECT_EQ(combat.GetEnemy(0), &goblin);
  EXPECT_EQ(combat.GetEnemy(1), &empty);
  EXPECT_EQ(combat.GetEnemy(2), &goblin);
  EXPECT_EQ(combat.GetEnemy(3), &empty);
}

TEST(CombatConstructorTest, StartsWithZeroAp) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_EQ(combat.GetCurrentAp(), 0);
  EXPECT_EQ(combat.GetNextAp(), 0);
}

TEST(CombatConstructorTest, EmptySlotIsDead) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  EXPECT_TRUE(combat.GetEnemy(1)->IsEmpty());
  EXPECT_FALSE(combat.GetEnemy(1)->IsAlive());
}

TEST(CombatConstructorTest, NextActsStartAsAlreadyDead) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &goblin, &empty};
  Combat combat(player, slots);

  for (int slot = 0; slot < Combat::kMaxEnemySlots; ++slot) {
    EXPECT_EQ(combat.GetNextAct(slot), EnemySkill::kAlreadyDead);
  }
}

TEST(CombatIsOverTest, IsNotOverWhileAnEnemyLives) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_FALSE(combat.IsOver());
}

TEST(CombatIsOverTest, IsOverWhenEverySlotIsEmpty) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  EXPECT_TRUE(combat.IsOver());
}

TEST(CombatIsOverTest, IsOverWhenThePlayerDies) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  player.TakeDamage(1000);

  EXPECT_TRUE(combat.IsOver());
}

TEST(CombatStartBattleTest, DrawsAHandOfHandSize) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);

  combat.StartBattle();

  EXPECT_EQ(player.GetDeck().GetHand().size(),
            static_cast<std::size_t>(Combat::kHandSize));
}

TEST(CombatStartBattleTest, HandComesFromTheDeck) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);

  combat.StartBattle();

  for (const Card& card : player.GetDeck().GetHand()) {
    EXPECT_EQ(card.GetName(), "Test card");
  }
}

TEST(CombatStartBattleTest, IntentsAreReadyAfterStart) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);

  combat.StartBattle();

  EXPECT_NE(combat.GetNextAct(0), EnemySkill::kAlreadyDead);
  EXPECT_EQ(combat.GetNextAct(1), EnemySkill::kAlreadyDead);
  EXPECT_EQ(combat.GetNextAct(2), EnemySkill::kAlreadyDead);
  EXPECT_EQ(combat.GetNextAct(3), EnemySkill::kAlreadyDead);
}

TEST(CombatStartBattleTest, GrantsTheDefaultApOnTheFirstRound) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);

  combat.StartBattle();

  EXPECT_EQ(combat.GetCurrentAp(), Combat::kDefaultApPerTurn);
  EXPECT_EQ(combat.GetNextAp(), Combat::kDefaultApPerTurn);
}

TEST(CombatUseCardTest, PlaysACardOnThePlayerAndSpendsAp) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const int cost = player.GetDeck().GetHand()[0].GetCost();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_TRUE(combat.UseCard(0, -1));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before - cost);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before - 1);
}

TEST(CombatUseCardTest, PlaysACardOnALivingEnemy) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const int cost = player.GetDeck().GetHand()[0].GetCost();

  EXPECT_TRUE(combat.UseCard(0, 0));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before - cost);
  EXPECT_TRUE(goblin.IsAlive());
}

TEST(CombatUseCardTest, RejectsAnInvalidCardIndex) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_FALSE(combat.UseCard(-1, -1));
  EXPECT_FALSE(combat.UseCard(99, -1));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before);
}

TEST(CombatUseCardTest, RejectsACardThatCostsMoreThanTheCurrentAp) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10, 5);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_FALSE(combat.UseCard(0, -1));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before);
}

TEST(CombatUseCardTest, RejectsAnEmptyTargetSlot) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_FALSE(combat.UseCard(0, 1));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before);
}

TEST(CombatUseCardTest, RejectsADeadTargetSlot) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);
  goblin.TakeDamage(1000);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_FALSE(combat.UseCard(0, 0));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before);
}

TEST(CombatUseCardTest, RejectsATargetSlotOutOfRange) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 10);
  combat.StartBattle();

  const int ap_before = combat.GetCurrentAp();
  const std::size_t hand_before = player.GetDeck().GetHand().size();

  EXPECT_FALSE(combat.UseCard(0, 4));
  EXPECT_FALSE(combat.UseCard(0, -2));

  EXPECT_EQ(combat.GetCurrentAp(), ap_before);
  EXPECT_EQ(player.GetDeck().GetHand().size(), hand_before);
}

TEST(CombatEndRoundTest, DrawsANewHand) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetDeck().GetHand().size(),
            static_cast<std::size_t>(Combat::kHandSize));
}

TEST(CombatEndRoundTest, ClearsEnemyShield) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();
  goblin.AddShield(10);

  combat.EndRound();

  EXPECT_EQ(goblin.GetShield(), 0);
}

TEST(CombatEndRoundTest, GrantsTheNextApForTheNewRound) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();
  ASSERT_TRUE(combat.UseCard(0, -1));

  combat.EndRound();

  EXPECT_EQ(combat.GetCurrentAp(), Combat::kDefaultApPerTurn);
  EXPECT_EQ(combat.GetNextAp(), Combat::kDefaultApPerTurn);
}

TEST(CombatEndRoundTest, RefreshesIntentsForTheNewRound) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_NE(combat.GetNextAct(0), EnemySkill::kAlreadyDead);
  EXPECT_EQ(combat.GetNextAct(1), EnemySkill::kAlreadyDead);
}

TEST(CombatEndRoundTest, DoesNothingWhenThePlayerIsAlreadyDead) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  Combat combat(player, slots);
  FillDeck(player, 20);
  combat.StartBattle();
  goblin.AddShield(10);
  player.TakeDamage(1000);

  combat.EndRound();

  EXPECT_EQ(goblin.GetShield(), 10);
}
