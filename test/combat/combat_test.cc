#include "combat/combat.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "card/effect_context.h"
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

namespace {

// Removes another card from the hand while the played card is being resolved,
// standing in for any effect that touches the deck mid-resolution.
class DiscardFirstCardEffect : public CardEffect {
 public:
  void Apply(EffectContext& context) const override {
    context.deck->DiscardPlayedCard(0);
  }

  std::unique_ptr<CardEffect> Clone() const override {
    return std::make_unique<DiscardFirstCardEffect>();
  }
};

}  // namespace

TEST(CombatUseCardTest, DiscardsThePlayedCardWhenAnEffectChangesTheHand) {
  Player player("Knight", 100, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&empty, &empty, &empty, &empty};
  Combat combat(player, slots);

  Card card_a("A", "A card for tests", CardRarity::kCommon, CardType::kSpecial,
              0, 0);
  Card card_b("B", "A card for tests", CardRarity::kCommon, CardType::kSpecial,
              0, 0);
  card_b.AddEffect(std::make_unique<DiscardFirstCardEffect>());
  Card card_c("C", "A card for tests", CardRarity::kCommon, CardType::kSpecial,
              0, 0);

  // StartBattle and Draw skip the shuffle, and Draw takes from the back of the
  // draw pile, so adding the cards in reverse gives hand = [A, B, C].
  Deck& deck = player.GetDeck();
  deck.AddCard(card_c);
  deck.AddCard(card_b);
  deck.AddCard(card_a);
  deck.StartBattle();
  deck.Draw(3);

  const std::vector<Card>& hand = player.GetDeck().GetHand();
  ASSERT_EQ(hand.size(), 3U);
  ASSERT_EQ(hand[0].GetName(), "A");
  ASSERT_EQ(hand[1].GetName(), "B");
  ASSERT_EQ(hand[2].GetName(), "C");

  ASSERT_TRUE(combat.UseCard(1, -1));

  ASSERT_EQ(hand.size(), 1U);
  EXPECT_EQ(hand[0].GetName(), "C");
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

namespace {

struct RecordedCall {
  Player* player = nullptr;
  Enemy* target = nullptr;
  const std::vector<Enemy*>* enemies = nullptr;
  Deck* deck = nullptr;
};

// Copies the context it receives instead of changing any game state, so the
// test can inspect what the combat code handed to the skill card.
class RecordingEffect : public CardEffect {
 public:
  explicit RecordingEffect(std::vector<RecordedCall>* log) : log_(log) {}

  void Apply(EffectContext& context) const override {
    RecordedCall call;
    call.player = context.player;
    call.target = context.target;
    call.enemies = context.enemies;
    call.deck = context.deck;
    log_->push_back(call);
  }

  std::unique_ptr<CardEffect> Clone() const override {
    return std::make_unique<RecordingEffect>(log_);
  }

 private:
  std::vector<RecordedCall>* log_;
};

// Test double for the skill-to-card lookup. Production keeps GetSkillCard
// empty until the enemy skill cards exist; this subclass supplies the card.
class SkillCardStub : public Combat {
 public:
  SkillCardStub(Player& player, const std::vector<Enemy*>& slots, Card* card)
      : Combat(player, slots), card_(card) {}

 protected:
  Card* GetSkillCard(EnemySkill skill) const override {
    static_cast<void>(skill);
    return card_;
  }

 private:
  Card* card_;
};

}  // namespace

TEST(CombatResolveOneSkillTest, AppliesTheSkillCardWithThePlayerAsVictim) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);
  std::vector<RecordedCall> log;
  Card card = MakeCard();
  card.AddEffect(std::make_unique<RecordingEffect>(&log));

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  SkillCardStub combat(player, slots, &card);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  ASSERT_EQ(log.size(), 1U);
  EXPECT_EQ(log[0].player, &player);
  EXPECT_EQ(log[0].target, &goblin);
}

TEST(CombatResolveOneSkillTest, PassesEachCasterSlotAsTheContextTarget) {
  Player player("Knight", 100, kSeed);
  Goblin first(1, kSeed);
  Goblin second(1, kSeed);
  EmptyEnemy empty(0, kSeed);
  std::vector<RecordedCall> log;
  Card card = MakeCard();
  card.AddEffect(std::make_unique<RecordingEffect>(&log));

  std::vector<Enemy*> slots = {&first, &empty, &second, &empty};
  SkillCardStub combat(player, slots, &card);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  ASSERT_EQ(log.size(), 2U);
  EXPECT_EQ(log[0].target, &first);
  EXPECT_EQ(log[1].target, &second);
}

TEST(CombatResolveOneSkillTest, PassesAllFourSlotsAndNoDeck) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);
  std::vector<RecordedCall> log;
  Card card = MakeCard();
  card.AddEffect(std::make_unique<RecordingEffect>(&log));

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  SkillCardStub combat(player, slots, &card);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  ASSERT_EQ(log.size(), 1U);
  ASSERT_NE(log[0].enemies, nullptr);
  EXPECT_EQ(log[0].enemies->size(),
            static_cast<std::size_t>(Combat::kMaxEnemySlots));
  EXPECT_EQ((*log[0].enemies)[0], &goblin);
  EXPECT_EQ((*log[0].enemies)[1], &empty);
  EXPECT_EQ(log[0].deck, nullptr);
}

TEST(CombatResolveOneSkillTest, SurvivesAMissingSkillCard) {
  Player player("Knight", 100, kSeed);
  Goblin goblin(1, kSeed);
  EmptyEnemy empty(0, kSeed);

  std::vector<Enemy*> slots = {&goblin, &empty, &empty, &empty};
  SkillCardStub combat(player, slots, nullptr);
  FillDeck(player, 20);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 100);
}

// std::max takes its arguments by const reference, so this odr-uses the
// constant and needs a definition. A static const member initialized in the
// class body has none, which is why this failed to link before the fix.
TEST(CombatConstantsTest, ConstantsAreUsableByReference) {
  EXPECT_EQ(std::max(Combat::kHandSize, Combat::kMaxEnemySlots), 5);
  EXPECT_EQ(std::max(Combat::kDefaultApPerTurn, 1), 3);
}
