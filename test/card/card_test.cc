#include "card/card.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
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

  void Apply(EffectContext& context) const override {
    EXPECT_EQ(&context, &expected_context_);
    calls_.emplace_back(id_, ++applications_);
  }

  std::unique_ptr<CardEffect> Clone() const override {
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
  void Apply(EffectContext&) const override {}

  std::unique_ptr<CardEffect> Clone() const override {
    throw std::runtime_error("test clone failure");
  }
};

class DestructionEffect : public CardEffect {
 public:
  explicit DestructionEffect(int& destructions) : destructions_(destructions) {}
  ~DestructionEffect() override { ++destructions_; }
  void Apply(EffectContext&) const override {}

  std::unique_ptr<CardEffect> Clone() const override {
    return std::make_unique<DestructionEffect>(destructions_);
  }

 private:
  int& destructions_;
};

class CardTest : public ::testing::Test {
 protected:
  void AddRecordingEffect(Card& card, int id) {
    card.AddEffect(
        std::make_unique<RecordingEffect>(id, calls_, clones_, context_));
  }

  void ExpectMetadataEqual(const Card& actual, const Card& expected) {
    EXPECT_EQ(actual.GetName(), expected.GetName());
    EXPECT_EQ(actual.GetDescription(), expected.GetDescription());
    EXPECT_EQ(actual.GetRarity(), expected.GetRarity());
    EXPECT_EQ(actual.GetType(), expected.GetType());
    EXPECT_EQ(actual.GetCost(), expected.GetCost());
    EXPECT_EQ(actual.GetValue(), expected.GetValue());
  }

  EffectCalls calls_;
  int clones_ = 0;
  EffectContext context_;
  Card card_{"Test card",
             "Metadata fixture",
             CardRarity::kVeryRare,
             CardType::kSpecial,
             3,
             7};
};

TEST_F(CardTest, ConstructorPreservesMetadata) {
  EXPECT_EQ(card_.GetName(), "Test card");
  EXPECT_EQ(card_.GetDescription(), "Metadata fixture");
  EXPECT_EQ(card_.GetRarity(), CardRarity::kVeryRare);
  EXPECT_EQ(card_.GetValue(), 7);
}

TEST_F(CardTest, PreservesPrimaryTypeAndApCost) {
  for (CardType type : {CardType::kAttack, CardType::kDefense, CardType::kHeal,
                        CardType::kSpecial}) {
    Card card("Type fixture", "", CardRarity::kCommon, type, 2, 0);
    EXPECT_EQ(card.GetType(), type);
    EXPECT_EQ(card.GetCost(), 2);
  }
  Card free_card("Zero cost fixture", "", CardRarity::kRare, CardType::kHeal, 0,
                 0);
  EXPECT_EQ(free_card.GetCost(), 0);
}

TEST_F(CardTest, EmptyCardHasNoImplicitEffects) {
  EXPECT_NO_THROW(card_.ApplyEffect(context_));
  EXPECT_TRUE(calls_.empty());
}

TEST_F(CardTest, AddingOneEffectTransfersOwnershipAndDispatchesIt) {
  auto effect = std::make_unique<RecordingEffect>(1, calls_, clones_, context_);
  card_.AddEffect(std::move(effect));
  EXPECT_EQ(effect, nullptr);
  EXPECT_TRUE(calls_.empty());
  EXPECT_EQ(clones_, 0);

  const Card& card = card_;
  card.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, MultipleEffectsExecuteInInsertionOrderOnEveryApplication) {
  AddRecordingEffect(card_, 3);
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  EXPECT_TRUE(calls_.empty());

  card_.ApplyEffect(context_);
  card_.ApplyEffect(context_);

  EXPECT_EQ(calls_,
            (EffectCalls{{3, 1}, {1, 1}, {2, 1}, {3, 2}, {1, 2}, {2, 2}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, NullEffectIsRejectedWithoutChangingExistingEffects) {
  AddRecordingEffect(card_, 1);
  EXPECT_THROW(card_.AddEffect(nullptr), std::invalid_argument);

  card_.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, CopyConstructorClonesStateIntoIndependentEffects) {
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  card_.ApplyEffect(context_);
  calls_.clear();

  Card copy(card_);
  ExpectMetadataEqual(copy, card_);
  EXPECT_EQ(clones_, 2);

  copy.ApplyEffect(context_);
  card_.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 2}, {2, 2}, {1, 2}, {2, 2}}));
}

TEST_F(CardTest, CopyRemainsUsableAfterOriginalIsDestroyed) {
  Card copy = [this]() {
    Card original("Scoped fixture", "", CardRarity::kCommon, CardType::kAttack,
                  1, 0);
    AddRecordingEffect(original, 1);
    return Card(original);
  }();

  EXPECT_EQ(clones_, 1);
  copy.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
}

TEST_F(CardTest, CopyAssignmentReplacesMetadataAndDeepCopiesEffects) {
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  Card assigned("Old fixture", "Old description", CardRarity::kEpic,
                CardType::kDefense, 1, 9);
  AddRecordingEffect(assigned, 9);

  EXPECT_EQ(&(assigned = card_), &assigned);
  ExpectMetadataEqual(assigned, card_);
  EXPECT_EQ(clones_, 2);

  assigned.ApplyEffect(context_);
  card_.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {2, 1}, {1, 1}, {2, 1}}));
}

TEST_F(CardTest, SelfCopyAssignmentPreservesEffectsAndState) {
  AddRecordingEffect(card_, 1);
  card_.ApplyEffect(context_);
  const Card& same_card = card_;

  card_ = same_card;

  card_.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {1, 2}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, CopyingAnEmptyCardClearsDestinationEffects) {
  Card empty_copy(card_);
  ExpectMetadataEqual(empty_copy, card_);
  AddRecordingEffect(card_, 1);

  card_ = empty_copy;

  card_.ApplyEffect(context_);
  EXPECT_TRUE(calls_.empty());
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, FailedCopyAssignmentLeavesDestinationUnchanged) {
  AddRecordingEffect(card_, 1);
  card_.AddEffect(std::make_unique<FailingCloneEffect>());
  Card destination("Original fixture", "Keep description", CardRarity::kRare,
                   CardType::kDefense, 1, 4);
  const Card expected_metadata(destination);
  AddRecordingEffect(destination, 9);

  EXPECT_THROW(destination = card_, std::runtime_error);

  ExpectMetadataEqual(destination, expected_metadata);
  destination.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{9, 1}}));
}

TEST_F(CardTest, MoveConstructorTransfersEffectsWithoutCloning) {
  static_assert(std::is_nothrow_move_constructible_v<Card>);
  const Card expected_metadata(card_);
  AddRecordingEffect(card_, 1);
  AddRecordingEffect(card_, 2);
  card_.ApplyEffect(context_);
  calls_.clear();

  Card moved(std::move(card_));

  ExpectMetadataEqual(moved, expected_metadata);
  moved.ApplyEffect(context_);
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
  Card moved("Old fixture", "", CardRarity::kCommon, CardType::kAttack, 0, 0);
  AddRecordingEffect(moved, 9);

  EXPECT_EQ(&(moved = std::move(card_)), &moved);

  ExpectMetadataEqual(moved, expected_metadata);
  moved.ApplyEffect(context_);
  EXPECT_EQ(calls_, (EffectCalls{{1, 1}}));
  EXPECT_EQ(clones_, 0);
}

TEST_F(CardTest, VectorOfCardsCanCopyCardsByValue) {
  AddRecordingEffect(card_, 1);
  std::vector<Card> cards;
  cards.push_back(card_);
  cards.push_back(card_);

  cards[0].ApplyEffect(context_);
  cards[1].ApplyEffect(context_);
  card_.ApplyEffect(context_);

  EXPECT_EQ(calls_, (EffectCalls{{1, 1}, {1, 1}, {1, 1}}));
  EXPECT_EQ(clones_, 2);
}

TEST_F(CardTest, OwnedEffectsAreDestroyedWithTheirCards) {
  int destructions = 0;
  {
    Card owner(card_);
    owner.AddEffect(std::make_unique<DestructionEffect>(destructions));
    {
      Card copy(owner);
      EXPECT_EQ(destructions, 0);
    }
    EXPECT_EQ(destructions, 1);
  }
  EXPECT_EQ(destructions, 2);
}
