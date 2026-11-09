#include "card/deck.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "card/Card.h"
#include "card/CardRarity.h"
#include "card/CardType.h"
#include "gtest/gtest.h"

// TDD contract assumptions for the unfinished Deck interface:
// - GetCards is the persistent collection; battle operations do not edit it.
// - StartBattle resets battle piles from that collection, with an empty hand.
// - Shuffle shuffles the draw pile; no opening-hand size/order is specified.
// - Draw appends up to count cards, recycling discards only when draw is empty.
// - Discard indexes are zero-based hand indexes; invalid indexes return false.
// Confirm these assumptions with the team before implementing Deck gameplay.

// These are test fixtures, not proposed gameplay cards.
Card MakeDeckTestCard(int id) {
  return Card("Fixture " + std::to_string(id),
              "Description " + std::to_string(id), CardRarity::C,
              CardType::Special, id, id + 10);
}

using CardMetadata =
    std::tuple<std::string, std::string, CardRarity, CardType, int, int>;

std::vector<CardMetadata> GetCardMetadata(const std::vector<Card>& cards) {
  std::vector<CardMetadata> result;
  for (const Card& card : cards) {
    result.emplace_back(card.getName(), card.getDescription(), card.getRarity(),
                        card.getType(), card.getCost(), card.getValue());
  }
  return result;
}

std::vector<CardMetadata> SortedCardMetadata(const std::vector<Card>& cards) {
  auto result = GetCardMetadata(cards);
  std::sort(result.begin(), result.end());
  return result;
}

class DeckTest : public ::testing::Test {
 protected:
  void SetUp() override {
    for (const Card& card : initial_cards_) {
      deck_.AddCard(card);
    }
  }

  static constexpr uint32_t kSeed = 12345;
  const std::vector<Card> initial_cards_{
      MakeDeckTestCard(1), MakeDeckTestCard(2), MakeDeckTestCard(3),
      MakeDeckTestCard(4)};
  Deck deck_{kSeed};
};

TEST(DeckConstructionTest, SeededDeckStartsWithEmptyCollectionAndHand) {
  const Deck deck(12345);
  EXPECT_TRUE(deck.GetCards().empty());
  EXPECT_TRUE(deck.GetHand().empty());
}

TEST(DeckConstructionTest, AddCardCopiesMetadataAndAllowsDuplicateCards) {
  Deck deck(12345);
  {
    const Card card = MakeDeckTestCard(1);
    deck.AddCard(card);
    deck.AddCard(card);
  }

  const std::vector<Card> expected{MakeDeckTestCard(1), MakeDeckTestCard(1)};
  EXPECT_EQ(GetCardMetadata(deck.GetCards()), GetCardMetadata(expected));
}

TEST_F(DeckTest, GetCardsExposesCollectionAsAConstReference) {
  static_assert(std::is_same_v<decltype(std::declval<const Deck&>().GetCards()),
                               const std::vector<Card>&>);
  const auto& cards = deck_.GetCards();
  EXPECT_EQ(GetCardMetadata(cards), GetCardMetadata(initial_cards_));

  deck_.AddCard(MakeDeckTestCard(5));
  auto expected = initial_cards_;
  expected.push_back(MakeDeckTestCard(5));
  EXPECT_EQ(GetCardMetadata(cards), GetCardMetadata(expected));
}

TEST_F(DeckTest, RemoveCardAcceptsFirstMiddleAndLastIndexes) {
  for (int index : {0, 1, 3}) {
    SCOPED_TRACE(index);
    Deck deck(kSeed);
    for (const Card& card : initial_cards_) {
      deck.AddCard(card);
    }
    auto expected = initial_cards_;
    expected.erase(expected.begin() + index);

    EXPECT_TRUE(deck.RemoveCard(index));
    EXPECT_EQ(GetCardMetadata(deck.GetCards()), GetCardMetadata(expected));
  }
}

TEST_F(DeckTest, RemoveCardRejectsInvalidIndexesWithoutChangingCollection) {
  for (int index : {-1, 4, 100}) {
    SCOPED_TRACE(index);
    EXPECT_FALSE(deck_.RemoveCard(index));
    EXPECT_EQ(GetCardMetadata(deck_.GetCards()),
              GetCardMetadata(initial_cards_));
  }

  Deck empty(kSeed);
  EXPECT_FALSE(empty.RemoveCard(0));
  empty.AddCard(MakeDeckTestCard(1));
  EXPECT_TRUE(empty.RemoveCard(0));
  EXPECT_TRUE(empty.GetCards().empty());
}

TEST_F(DeckTest, StartBattleMakesCollectionDrawableAndLeavesHandEmpty) {
  deck_.StartBattle();

  EXPECT_TRUE(deck_.GetHand().empty());
  EXPECT_EQ(GetCardMetadata(deck_.GetCards()), GetCardMetadata(initial_cards_));
  deck_.Draw(static_cast<int>(initial_cards_.size()));
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
  EXPECT_EQ(GetCardMetadata(deck_.GetCards()), GetCardMetadata(initial_cards_));
}

TEST_F(DeckTest, StartBattleResetsPreviousDrawHandAndDiscardPiles) {
  deck_.StartBattle();
  deck_.Draw(2);
  ASSERT_EQ(deck_.GetHand().size(), 2u);
  ASSERT_TRUE(deck_.DiscardPlayedCard(0));

  deck_.StartBattle();

  EXPECT_TRUE(deck_.GetHand().empty());
  deck_.Draw(static_cast<int>(initial_cards_.size()) + 1);
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
}

TEST_F(DeckTest, DrawAppendsRequestedNumberWithoutReplacingExistingHand) {
  deck_.StartBattle();
  deck_.Draw(1);
  ASSERT_EQ(deck_.GetHand().size(), 1u);
  const auto first = GetCardMetadata(deck_.GetHand()).front();

  deck_.Draw(2);

  ASSERT_EQ(deck_.GetHand().size(), 3u);
  EXPECT_EQ(GetCardMetadata(deck_.GetHand()).front(), first);
  deck_.Draw(1);
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
}

TEST(DeckConstructionTest, DrawingFromEmptyBattleLeavesHandEmpty) {
  Deck deck(12345);
  deck.StartBattle();
  deck.Draw(2);
  EXPECT_TRUE(deck.GetHand().empty());
  EXPECT_TRUE(deck.GetCards().empty());
}

TEST_F(DeckTest, DrawStopsWhenDrawAndDiscardPilesAreEmpty) {
  deck_.StartBattle();
  deck_.Draw(static_cast<int>(initial_cards_.size()) + 2);
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
  const auto exhausted_hand = GetCardMetadata(deck_.GetHand());

  deck_.Draw(1);

  EXPECT_EQ(GetCardMetadata(deck_.GetHand()), exhausted_hand);
}

TEST_F(DeckTest, ShuffleIsReproducibleWithIdenticalSeedAndInitialDeck) {
  Deck same_seed(kSeed);
  for (const Card& card : initial_cards_) {
    same_seed.AddCard(card);
  }
  deck_.StartBattle();
  same_seed.StartBattle();
  deck_.Shuffle();
  same_seed.Shuffle();
  deck_.Draw(static_cast<int>(initial_cards_.size()));
  same_seed.Draw(static_cast<int>(initial_cards_.size()));

  EXPECT_EQ(GetCardMetadata(deck_.GetHand()),
            GetCardMetadata(same_seed.GetHand()));
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
  EXPECT_EQ(GetCardMetadata(deck_.GetCards()), GetCardMetadata(initial_cards_));
  // No hard-coded permutation or assertion that different seeds must differ.
}

TEST_F(DeckTest, DiscardPlayedCardRemovesSelectedHandCardAndMakesItDrawable) {
  deck_.StartBattle();
  deck_.Draw(static_cast<int>(initial_cards_.size()));
  ASSERT_EQ(deck_.GetHand().size(), initial_cards_.size());
  auto expected_hand = GetCardMetadata(deck_.GetHand());
  const auto played = expected_hand[1];
  expected_hand.erase(expected_hand.begin() + 1);

  EXPECT_TRUE(deck_.DiscardPlayedCard(1));

  EXPECT_EQ(GetCardMetadata(deck_.GetHand()), expected_hand);
  deck_.Draw(1);
  expected_hand.push_back(played);
  EXPECT_EQ(GetCardMetadata(deck_.GetHand()), expected_hand);
  EXPECT_EQ(GetCardMetadata(deck_.GetCards()), GetCardMetadata(initial_cards_));
}

TEST_F(DeckTest, DiscardPlayedCardRejectsInvalidIndexesWithoutChangingHand) {
  deck_.StartBattle();
  EXPECT_FALSE(deck_.DiscardPlayedCard(0));
  deck_.Draw(2);
  ASSERT_EQ(deck_.GetHand().size(), 2u);
  const auto hand = GetCardMetadata(deck_.GetHand());

  for (int index : {-1, 2, 100}) {
    SCOPED_TRACE(index);
    EXPECT_FALSE(deck_.DiscardPlayedCard(index));
    EXPECT_EQ(GetCardMetadata(deck_.GetHand()), hand);
  }
}

TEST_F(DeckTest, DiscardHandMovesAllCardsToDiscardForLaterDrawing) {
  deck_.StartBattle();
  deck_.Draw(static_cast<int>(initial_cards_.size()));
  ASSERT_EQ(deck_.GetHand().size(), initial_cards_.size());

  deck_.DiscardHand();

  EXPECT_TRUE(deck_.GetHand().empty());
  deck_.DiscardHand();
  deck_.Draw(static_cast<int>(initial_cards_.size()) + 1);
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
  EXPECT_EQ(GetCardMetadata(deck_.GetCards()), GetCardMetadata(initial_cards_));
}

TEST_F(DeckTest, DrawUsesRemainingDrawPileThenReshufflesDiscardsInSameRequest) {
  deck_.StartBattle();
  deck_.Draw(2);
  ASSERT_EQ(deck_.GetHand().size(), 2u);
  const auto discarded = SortedCardMetadata(deck_.GetHand());
  deck_.DiscardHand();

  // Two cards remain in draw and two in discard. One Draw crosses the boundary.
  deck_.Draw(4);

  ASSERT_EQ(deck_.GetHand().size(), 4u);
  EXPECT_EQ(SortedCardMetadata(deck_.GetHand()),
            SortedCardMetadata(initial_cards_));
  auto hand = GetCardMetadata(deck_.GetHand());
  std::vector<CardMetadata> recycled(hand.begin() + 2, hand.end());
  std::sort(recycled.begin(), recycled.end());
  EXPECT_EQ(recycled, discarded);

  // Discard must have been emptied, so another draw cannot duplicate a card.
  deck_.Draw(1);
  EXPECT_EQ(GetCardMetadata(deck_.GetHand()), hand);
}
