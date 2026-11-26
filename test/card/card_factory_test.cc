#include "card/card_factory.h"

#include <string>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/deck.h"
#include "card/effect_context.h"
#include "card/target_type.h"
#include "enemy/empty_enemy.h"
#include "enemy/enemy.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {

void ExpectMetadata(const Card& card, const std::string& name,
                    CardRarity rarity, CardType type, int cost,
                    const std::vector<TargetType>& targets) {
  EXPECT_EQ(card.GetName(), name);
  EXPECT_EQ(card.GetRarity(), rarity);
  EXPECT_EQ(card.GetType(), type);
  EXPECT_EQ(card.GetCost(), cost);
  EXPECT_EQ(card.GetEffectTargetTypes(), targets);
}

std::vector<EffectContext> LifeDrainContexts(Player& player, Enemy& enemy) {
  std::vector<EffectContext> contexts(2);
  contexts[0].caster_player = &player;
  contexts[0].target_enemy = &enemy;
  contexts[1].caster_player = &player;
  return contexts;
}

std::vector<Card> CreateFactoryCards() {
  return {CardFactory::CreateStrike(), CardFactory::CreateDefend(),
          CardFactory::CreateLifeDrain(), CardFactory::CreateWhirlwind()};
}

Deck CreateDeckWithFactoryCards() {
  Deck deck(42);
  for (const Card& card : CreateFactoryCards()) {
    deck.AddCard(card);
  }
  return deck;
}

const Card* FindCard(const std::vector<Card>& cards, const std::string& name) {
  for (const Card& card : cards) {
    if (card.GetName() == name) {
      return &card;
    }
  }
  return nullptr;
}

void ExpectCardWorks(const Card& card, int enemy_hp, int other_enemy_hp,
                     int player_hp, int player_shield) {
  SCOPED_TRACE(card.GetName());
  Player player("Knight", 30, 42);
  player.TakeDamage(10);
  Goblin enemy(1, 42);
  Goblin other_enemy(1, 43);

  const auto target_types = card.GetEffectTargetTypes();
  ASSERT_FALSE(target_types.empty());
  std::vector<EffectContext> contexts(target_types.size());
  for (EffectContext& context : contexts) {
    context.caster_player = &player;
  }
  contexts[0].target_enemy = &enemy;
  contexts[0].enemy_targets = {&enemy, &other_enemy};

  card.ApplyEffect(contexts);

  EXPECT_EQ(enemy.GetHp(), enemy_hp);
  EXPECT_EQ(other_enemy.GetHp(), other_enemy_hp);
  EXPECT_EQ(player.GetHp(), player_hp);
  EXPECT_EQ(player.GetShield(), player_shield);
}

void ExpectAllFourCardsWork(const std::vector<Card>& cards) {
  ASSERT_EQ(cards.size(), 4U);
  const Card* strike = FindCard(cards, "Strike");
  const Card* defend = FindCard(cards, "Defend");
  const Card* life_drain = FindCard(cards, "Life Drain");
  const Card* whirlwind = FindCard(cards, "Whirlwind");
  ASSERT_NE(strike, nullptr);
  ASSERT_NE(defend, nullptr);
  ASSERT_NE(life_drain, nullptr);
  ASSERT_NE(whirlwind, nullptr);

  ExpectMetadata(*strike, "Strike", CardRarity::kCommon, CardType::kAttack, 1,
                 {TargetType::kSingleEnemy});
  ExpectMetadata(*defend, "Defend", CardRarity::kCommon, CardType::kDefense, 1,
                 {TargetType::kSelf});
  ExpectMetadata(*life_drain, "Life Drain", CardRarity::kRare,
                 CardType::kAttack, 2,
                 {TargetType::kSingleEnemy, TargetType::kSelf});
  ExpectMetadata(*whirlwind, "Whirlwind", CardRarity::kRare, CardType::kAttack,
                 1, {TargetType::kAllEnemies});

  ExpectCardWorks(*strike, 25, 30, 20, 0);
  ExpectCardWorks(*defend, 30, 30, 20, 5);
  ExpectCardWorks(*life_drain, 25, 30, 22, 0);
  ExpectCardWorks(*whirlwind, 27, 27, 20, 0);
}

}  // namespace

TEST(StrikeFactoryTest, HasExpectedMetadataAndTarget) {
  const Card card = CardFactory::CreateStrike();

  ExpectMetadata(card, "Strike", CardRarity::kCommon, CardType::kAttack, 1,
                 {TargetType::kSingleEnemy});
}

TEST(StrikeFactoryTest, DealsFiveDamageToTheSelectedEnemy) {
  Player player("Knight", 30, 42);
  Goblin enemy(1, 42);
  EffectContext context;
  context.caster_player = &player;
  context.target_enemy = &enemy;

  CardFactory::CreateStrike().ApplyEffect(context);

  EXPECT_EQ(enemy.GetHp(), 25);
}

TEST(StrikeFactoryTest, RespectsEnemyShield) {
  Player player("Knight", 30, 42);
  Goblin enemy(1, 42);
  enemy.AddShield(3);
  EffectContext context;
  context.caster_player = &player;
  context.target_enemy = &enemy;

  CardFactory::CreateStrike().ApplyEffect(context);

  EXPECT_EQ(enemy.GetShield(), 0);
  EXPECT_EQ(enemy.GetHp(), 28);
}

TEST(StrikeFactoryTest, LeavesOtherEnemiesUnharmed) {
  Player player("Knight", 30, 42);
  Goblin selected(1, 42);
  Goblin other(1, 43);
  EffectContext context;
  context.caster_player = &player;
  context.target_enemy = &selected;
  context.enemy_targets = {&selected, &other};

  CardFactory::CreateStrike().ApplyEffect(context);

  EXPECT_EQ(selected.GetHp(), 25);
  EXPECT_EQ(other.GetHp(), 30);
}

TEST(DefendFactoryTest, HasExpectedMetadataAndTarget) {
  const Card card = CardFactory::CreateDefend();

  ExpectMetadata(card, "Defend", CardRarity::kCommon, CardType::kDefense, 1,
                 {TargetType::kSelf});
}

TEST(DefendFactoryTest, AddsFiveShieldToTheCaster) {
  Player player("Knight", 30, 42);
  EffectContext context;
  context.caster_player = &player;

  CardFactory::CreateDefend().ApplyEffect(context);

  EXPECT_EQ(player.GetShield(), 5);
}

TEST(DefendFactoryTest, StacksWithExistingShield) {
  Player player("Knight", 30, 42);
  player.AddShield(3);
  EffectContext context;
  context.caster_player = &player;

  CardFactory::CreateDefend().ApplyEffect(context);

  EXPECT_EQ(player.GetShield(), 8);
}

TEST(LifeDrainFactoryTest, HasExpectedMetadataAndTargetsInOrder) {
  const Card card = CardFactory::CreateLifeDrain();

  ExpectMetadata(card, "Life Drain", CardRarity::kRare, CardType::kAttack, 2,
                 {TargetType::kSingleEnemy, TargetType::kSelf});
}

TEST(LifeDrainFactoryTest, DealsFiveDamageToTheSelectedEnemy) {
  Player player("Knight", 30, 42);
  Goblin enemy(1, 42);
  auto contexts = LifeDrainContexts(player, enemy);

  CardFactory::CreateLifeDrain().ApplyEffect(contexts);

  EXPECT_EQ(enemy.GetHp(), 25);
}

TEST(LifeDrainFactoryTest, RestoresTwoHpToTheCaster) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);
  Goblin enemy(1, 42);
  auto contexts = LifeDrainContexts(player, enemy);

  CardFactory::CreateLifeDrain().ApplyEffect(contexts);

  EXPECT_EQ(player.GetHp(), 22);
}

TEST(LifeDrainFactoryTest, CapsHealingAtMaxHp) {
  Player player("Knight", 30, 42);
  player.TakeDamage(1);
  Goblin enemy(1, 42);
  auto contexts = LifeDrainContexts(player, enemy);

  CardFactory::CreateLifeDrain().ApplyEffect(contexts);

  EXPECT_EQ(player.GetHp(), player.GetMaxHp());
}

TEST(LifeDrainFactoryTest, StillHealsTheCasterAfterKillingTheEnemy) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);
  Goblin enemy(1, 42);
  enemy.TakeDamage(25);
  auto contexts = LifeDrainContexts(player, enemy);

  CardFactory::CreateLifeDrain().ApplyEffect(contexts);

  EXPECT_FALSE(enemy.IsAlive());
  EXPECT_EQ(enemy.GetHp(), 0);
  EXPECT_EQ(player.GetHp(), 22);
}

TEST(WhirlwindFactoryTest, HasExpectedMetadataAndTarget) {
  const Card card = CardFactory::CreateWhirlwind();

  ExpectMetadata(card, "Whirlwind", CardRarity::kRare, CardType::kAttack, 1,
                 {TargetType::kAllEnemies});
}

TEST(WhirlwindFactoryTest, DealsThreeDamageToAllLivingEnemies) {
  Player player("Knight", 30, 42);
  Goblin first(1, 42);
  Goblin second(1, 43);
  EffectContext context;
  context.caster_player = &player;
  context.enemy_targets = {&first, &second};

  CardFactory::CreateWhirlwind().ApplyEffect(context);

  EXPECT_EQ(first.GetHp(), 27);
  EXPECT_EQ(second.GetHp(), 27);
}

TEST(WhirlwindFactoryTest, SkipsDeadEnemiesAndEmptySlots) {
  Player player("Knight", 30, 42);
  Goblin living(1, 42);
  Goblin dead(1, 43);
  dead.TakeDamage(30);
  EmptyEnemy empty(0, 44);
  EffectContext context;
  context.caster_player = &player;
  context.enemy_targets = {&dead, &empty, nullptr, &living};

  CardFactory::CreateWhirlwind().ApplyEffect(context);

  EXPECT_EQ(living.GetHp(), 27);
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_FALSE(dead.IsAlive());
  EXPECT_EQ(empty.GetHp(), 0);
  EXPECT_TRUE(empty.IsEmpty());
}

TEST(WhirlwindFactoryTest, LeavesTheCasterUnharmed) {
  Player player("Knight", 30, 42);
  Goblin enemy(1, 42);
  EffectContext context;
  context.caster_player = &player;
  context.enemy_targets = {&enemy};
  context.player_targets = {&player};

  CardFactory::CreateWhirlwind().ApplyEffect(context);

  EXPECT_EQ(enemy.GetHp(), 27);
  EXPECT_EQ(player.GetHp(), 30);
}

TEST(CardFactoryDeckTest, AddsAllFourCardsToTheCollection) {
  const Deck deck = CreateDeckWithFactoryCards();

  const auto& cards = deck.GetCards();
  ASSERT_EQ(cards.size(), 4U);
  EXPECT_NE(FindCard(cards, "Strike"), nullptr);
  EXPECT_NE(FindCard(cards, "Defend"), nullptr);
  EXPECT_NE(FindCard(cards, "Life Drain"), nullptr);
  EXPECT_NE(FindCard(cards, "Whirlwind"), nullptr);
}

TEST(CardFactoryDeckTest, DrawnCardsKeepTheirMetadataAndEffects) {
  Deck deck = CreateDeckWithFactoryCards();
  deck.StartBattle();
  deck.Shuffle();
  deck.Draw(4);

  ExpectAllFourCardsWork(deck.GetHand());
  for (const Card& drawn : deck.GetHand()) {
    const Card* original = FindCard(deck.GetCards(), drawn.GetName());
    ASSERT_NE(original, nullptr);
    EXPECT_EQ(drawn.GetDescription(), original->GetDescription());
    EXPECT_EQ(drawn.GetValue(), original->GetValue());
  }
}

TEST(CardFactoryDeckTest, CopiedCardsKeepTheirEffects) {
  std::vector<Card> copies;
  {
    const auto originals = CreateFactoryCards();
    for (const Card& original : originals) {
      copies.push_back(original);
    }
  }

  ExpectAllFourCardsWork(copies);
}

TEST(CardFactoryDeckTest, DiscardedCardsKeepTheirEffectsAfterReshuffling) {
  Deck deck = CreateDeckWithFactoryCards();
  deck.StartBattle();
  deck.Draw(4);
  ASSERT_EQ(deck.GetHand().size(), 4U);

  ASSERT_TRUE(deck.DiscardPlayedCard(0));
  deck.DiscardHand();
  ASSERT_TRUE(deck.GetHand().empty());
  deck.Draw(4);

  ExpectAllFourCardsWork(deck.GetHand());
}
