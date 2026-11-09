#include <algorithm>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "card/Card.h"
#include "card/CardRarity.h"
#include "card/CardType.h"
#include "card/card_effect.h"
#include "card/effect_context.h"
#include "gtest/gtest.h"

using EffectCalls = std::vector<std::pair<int, int>>;

// Clones share only the test's observation log/counter/context. Each effect
// owns its application count, so a shallow copy changes the recorded results.
class RecordingEffect : public CardEffect {
 public:
  RecordingEffect(int id, EffectCalls& calls, int& clones,
                  const EffectContext& expected_context)
      : id_(id),
        calls_(calls),
        clones_(clones),
        expected_context_(expected_context) {}

  void apply(EffectContext& context) const override {
    EXPECT_EQ(&context, &expected_context_);
    calls_.emplace_back(id_, ++applications_);
  }

  std::unique_ptr<CardEffect> clone() const override {
    ++clones_;
    return std::make_unique<RecordingEffect>(*this);
  }

 private:
  int id_;
  EffectCalls& calls_;
  int& clones_;
  const EffectContext& expected_context_;
  mutable int applications_ = 0;
};

class FailingCloneEffect : public CardEffect {
 public:
  void apply(EffectContext&) const override {}

  std::unique_ptr<CardEffect> clone() const override {
    throw std::runtime_error("test clone failure");
  }
};

class DestructionEffect : public CardEffect {
 public:
  explicit DestructionEffect(int& destructions) : destructions_(destructions) {}
  ~DestructionEffect() override { ++destructions_; }
  void apply(EffectContext&) const override {}

  std::unique_ptr<CardEffect> clone() const override {
    return std::make_unique<DestructionEffect>(destructions_);
  }

 private:
  int& destructions_;
};

class CardTest : public ::testing::Test {
 protected:
  void AddRecordingEffect(Card& card, int id) {
    card.addEffect(
        std::make_unique<RecordingEffect>(id, calls_, clones_, context_));
  }

  void ExpectMetadataEqual(const Card& actual, const Card& expected) {
    EXPECT_EQ(actual.getName(), expected.getName());
    EXPECT_EQ(actual.getDescription(), expected.getDescription());
    EXPECT_EQ(actual.getRarity(), expected.getRarity());
    EXPECT_EQ(actual.getType(), expected.getType());
    EXPECT_EQ(actual.getCost(), expected.getCost());
    EXPECT_EQ(actual.getValue(), expected.getValue());
  }

  EffectCalls calls_;
  int clones_ = 0;
  EffectContext context_;
  Card card_{"Test card", "Metadata fixture", CardRarity::VR, CardType::Special,
             3, 7};
};

TEST_F(CardTest, ConstructorPreservesMetadata) {
  EXPECT_EQ(card_.getName(), "Test card");
  EXPECT_EQ(card_.getDescription(), "Metadata fixture");
  EXPECT_EQ(card_.getRarity(), CardRarity::VR);
  EXPECT_EQ(card_.getValue(), 7);
}

TEST_F(CardTest, PreservesPrimaryTypeAndApCost) {
  for (CardType type : {CardType::Attack, CardType::Defense, CardType::Heal,
                        CardType::Special}) {
    Card card("Type fixture", "", CardRarity::C, type, 2, 0);
    EXPECT_EQ(card.getType(), type);
    EXPECT_EQ(card.getCost(), 2);
  }
  Card free_card("Zero cost fixture", "", CardRarity::R, CardType::Heal, 0, 0);
  EXPECT_EQ(free_card.getCost(), 0);
}

TEST_F(CardTest, EmptyCardHasNoImplicitEffects) {
  EXPECT_NO_THROW(card_.applyEffect(context_));
  EXPECT_TRUE(calls_.empty());
}

TEST_F(CardTest, AddingOneEffectTransfersOwnershipAndDispatchesIt) {
  auto effect = std::make_unique<RecordingEffect>(1, calls_, clones_, context_);
  card_.addEffect(std::move(effect));
  EXPECT_EQ(effect, nullptr);
  EXPECT_TRUE(calls_.empty());
  EXPECT_EQ(clones_, 0);

  const Card& card = card_;
  card.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, MultipleEffectsExecuteInInsertionOrderOnEveryApplication) {
  AddRecordingEffect(card_, 3);
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  EXPECT_TRUE(calls_.empty());

  card_.applyEffect(context_);
  card_.applyEffect(context_);

  EXPECT_EQ(calls_, (EffectCalls{{3, 1}, {1, 1}, {2, 1},
                                {3, 2}, {1, 2}, {2, 2}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, NullEffectIsRejectedWithoutChangingExistingEffects) {
  AddRecordingEffect(card_, 1);
  EXPECT_THROW(card_.addEffect(nullptr), std::invalid_argument);

  card_.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, CopyConstructorClonesStateIntoIndependentEffects) {
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  card_.applyEffect(context_);
  calls_.clear();

  Card copy(card_);
  ExpectMetadataEqual(copy, card_);
  EXPECT_EQ(clones_, 2);

  copy.applyEffect(context_);
  card_.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 2}, {2, 2}, {1, 2}, {2, 2}}));
}

TEST_F(CardTest, CopyRemainsUsableAfterOriginalIsDestroyed) {
  Card copy = [this]() {
    Card original("Scoped fixture", "", CardRarity::C, CardType::Attack, 1, 0);
    AddRecordingEffect(original, 1);
    return Card(original);
  }();

  EXPECT_EQ(clones_, 1);
  copy.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, CopyAssignmentReplacesMetadataAndDeepCopiesEffects) {
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  Card assigned("Old fixture", "Old description", CardRarity::E,
                CardType::Defense, 1, 9);
  AddRecordingEffect(assigned, 9);

  EXPECT_EQ(&(assigned = card_), &assigned);
  ExpectMetadataEqual(assigned, card_);
  EXPECT_EQ(clones_, 2);

  assigned.applyEffect(context_);
  card_.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {2, 1}, {1, 1}, {2, 1}}));
}

TEST_F(CardTest, SelfCopyAssignmentPreservesEffectsAndState) {
  AddRecordingEffect(card_, 1);
  card_.applyEffect(context_);
  const Card& same_card = card_;

  card_ = same_card;

  card_.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {1, 2}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, CopyingAnEmptyCardClearsDestinationEffects) {
  Card empty_copy(card_);
  ExpectMetadataEqual(empty_copy, card_);
  AddRecordingEffect(card_, 1);

  card_ = empty_copy;

  card_.applyEffect(context_);
  EXPECT_TRUE(calls_.empty());
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, FailedCopyAssignmentLeavesDestinationUnchanged) {
  AddRecordingEffect(card_, 1);
  card_.addEffect(std::make_unique<FailingCloneEffect>());
  Card destination("Original fixture", "Keep description", CardRarity::R,
                   CardType::Defense, 1, 4);
  const Card expected_metadata(destination);
  AddRecordingEffect(destination, 9);

  EXPECT_THROW(destination = card_, std::runtime_error);

  ExpectMetadataEqual(destination, expected_metadata);
  destination.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{9, 1}}));
}

TEST_F(CardTest, MoveConstructorTransfersEffectsWithoutCloning) {
  static_assert(std::is_nothrow_move_constructible_v<Card>);
  const Card expected_metadata(card_);
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  card_.applyEffect(context_);
  calls_.clear();

  Card moved(std::move(card_));

  ExpectMetadataEqual(moved, expected_metadata);
  moved.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 2}, {2, 2}}));
  EXPECT_EQ(clones_, 0);
  // A moved-from card remains assignable; its old metadata is unspecified.
  card_ = expected_metadata;
  ExpectMetadataEqual(card_, expected_metadata);
}

TEST_F(CardTest, MoveAssignmentReplacesEffectsWithoutCloning) {
  static_assert(std::is_nothrow_move_assignable_v<Card>);
  const Card expected_metadata(card_);
  AddRecordingEffect(card_, 1);
  Card moved("Old fixture", "", CardRarity::C, CardType::Attack, 0, 0);
  AddRecordingEffect(moved, 9);

  EXPECT_EQ(&(moved = std::move(card_)), &moved);

  ExpectMetadataEqual(moved, expected_metadata);
  moved.applyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, VectorOfCardsCanCopyCardsByValue) {
  AddRecordingEffect(card_, 1);
  std::vector<Card> cards;
  cards.push_back(card_);
  cards.push_back(card_);

  cards[0].applyEffect(context_);
  cards[1].applyEffect(context_);
  card_.applyEffect(context_);

  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {1, 1}, {1, 1}}));
  EXPECT_EQ(clones_, 2);
}

TEST_F(CardTest, OwnedEffectsAreDestroyedWithTheirCards) {
  int destructions = 0;
  {
    Card owner(card_);
    owner.addEffect(std::make_unique<DestructionEffect>(destructions));
    {
      Card copy(owner);
      EXPECT_EQ(destructions, 0);
    }
    EXPECT_EQ(destructions, 1);
  }
  EXPECT_EQ(destructions, 2);
}
