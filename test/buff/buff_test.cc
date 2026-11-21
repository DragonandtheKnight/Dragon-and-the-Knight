#include "buff/buff.h"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "gtest/gtest.h"

namespace {

Card MakeBuffCard(const std::string& name, const std::string& description) {
  return Card(name, description, CardRarity::kCommon, CardType::kSpecial, 0, 0);
}

// Observes accidental execution without changing player or enemy state.
class CountingEffect : public CardEffect {
 public:
  explicit CountingEffect(int* calls) : calls_(calls) {}

  void Apply(EffectContext&) const override { ++(*calls_); }

  std::unique_ptr<CardEffect> Clone() const override {
    return std::make_unique<CountingEffect>(calls_);
  }

 private:
  int* calls_;
};

}  // namespace

TEST(BuffConstructorTest, StartsWithNoActiveBuffs) {
  const Buff buffs;

  EXPECT_TRUE(buffs.GetBuffs().empty());
  EXPECT_EQ(buffs.GetRemainingRounds("Missing buff"), 0);
}

TEST(BuffAddTest, AddsOneGroupWithTheCardsMetadataAndRequestedCount) {
  Buff buffs;
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");

  buffs.AddBuff(card, 5);

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].description, "Adds AP when triggered.");
  EXPECT_EQ(info[0].remaining_rounds, 5);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 5);
}

TEST(BuffAddTest, SameNameFromAnotherCardExtendsTheExistingGroup) {
  Buff buffs;
  const Card first = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  const Card second = MakeBuffCard("Energy tick", "Adds AP when triggered.");

  buffs.AddBuff(first, 3);
  buffs.AddBuff(second, 5);

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].remaining_rounds, 8);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 8);
}

TEST(BuffAddTest, DifferentNamesStaySeparateEvenWithTheSameCardType) {
  Buff buffs;
  const Card energy = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  const Card healing =
      MakeBuffCard("Healing tick", "Restores HP when triggered.");

  buffs.AddBuff(energy, 5);
  buffs.AddBuff(healing, 3);

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 2U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].description, "Adds AP when triggered.");
  EXPECT_EQ(info[0].remaining_rounds, 5);
  EXPECT_EQ(info[1].name, "Healing tick");
  EXPECT_EQ(info[1].description, "Restores HP when triggered.");
  EXPECT_EQ(info[1].remaining_rounds, 3);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 5);
  EXPECT_EQ(buffs.GetRemainingRounds("Healing tick"), 3);
}

TEST(BuffAddTest, ExtendingAnEarlierGroupKeepsTheGroupOrder) {
  Buff buffs;
  const Card energy = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  const Card healing =
      MakeBuffCard("Healing tick", "Restores HP when triggered.");

  buffs.AddBuff(energy, 2);
  buffs.AddBuff(healing, 3);
  buffs.AddBuff(energy, 4);

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 2U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].remaining_rounds, 6);
  EXPECT_EQ(info[1].name, "Healing tick");
  EXPECT_EQ(info[1].remaining_rounds, 3);
}

TEST(BuffAddTest, ZeroCountDoesNotCreateAnEmptyGroup) {
  Buff buffs;
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");

  buffs.AddBuff(card, 0);

  EXPECT_TRUE(buffs.GetBuffs().empty());
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 0);
}

TEST(BuffAddTest, NegativeCountDoesNotCreateAnEmptyGroup) {
  Buff buffs;
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");

  buffs.AddBuff(card, -2);

  EXPECT_TRUE(buffs.GetBuffs().empty());
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 0);
}

TEST(BuffAddTest, InvalidCountsLeaveExistingGroupsUnchanged) {
  Buff buffs;
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  buffs.AddBuff(card, 5);

  buffs.AddBuff(card, 0);
  buffs.AddBuff(card, -2);

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].remaining_rounds, 5);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 5);
}

TEST(BuffAddTest, AddingACardDoesNotExecuteItsEffect) {
  int calls = 0;
  Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  card.AddEffect(std::make_unique<CountingEffect>(&calls));
  Buff buffs;

  buffs.AddBuff(card, 5);

  EXPECT_EQ(calls, 0);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 5);
}

TEST(BuffAddTest, StoredCardsDoNotFollowSourceCardReassignment) {
  Buff buffs;
  Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  buffs.AddBuff(card, 5);

  card = MakeBuffCard("Replacement", "This is not the stored buff.");

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].description, "Adds AP when triggered.");
  EXPECT_EQ(info[0].remaining_rounds, 5);
  EXPECT_EQ(buffs.GetRemainingRounds("Replacement"), 0);
}

TEST(BuffQueryTest, MissingNameReturnsZeroWithoutCreatingAGroup) {
  Buff buffs;
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  buffs.AddBuff(card, 5);
  const Buff& view = buffs;

  EXPECT_EQ(view.GetRemainingRounds("Missing buff"), 0);

  const std::vector<BuffInfo> info = view.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].remaining_rounds, 5);
}

TEST(BuffQueryTest, ConstQueriesDoNotConsumeCardsOrExecuteEffects) {
  int calls = 0;
  Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  card.AddEffect(std::make_unique<CountingEffect>(&calls));
  Buff buffs;
  buffs.AddBuff(card, 5);
  const Buff& view = buffs;

  const std::vector<BuffInfo> first = view.GetBuffs();
  EXPECT_EQ(view.GetRemainingRounds("Energy tick"), 5);
  const std::vector<BuffInfo> second = view.GetBuffs();
  EXPECT_EQ(view.GetRemainingRounds("Energy tick"), 5);

  ASSERT_EQ(first.size(), 1U);
  ASSERT_EQ(second.size(), 1U);
  EXPECT_EQ(first[0].name, "Energy tick");
  EXPECT_EQ(first[0].description, "Adds AP when triggered.");
  EXPECT_EQ(first[0].remaining_rounds, 5);
  EXPECT_EQ(second[0].name, first[0].name);
  EXPECT_EQ(second[0].description, first[0].description);
  EXPECT_EQ(second[0].remaining_rounds, 5);
  EXPECT_EQ(calls, 0);
}

TEST(BuffQueryTest, ChangingReturnedInfoDoesNotChangeStoredBuffs) {
  Buff buffs;
  static_assert(
      std::is_same_v<decltype(buffs.GetBuffs()), std::vector<BuffInfo>>,
      "GetBuffs must return a value snapshot.");
  const Card card = MakeBuffCard("Energy tick", "Adds AP when triggered.");
  buffs.AddBuff(card, 5);

  std::vector<BuffInfo> snapshot = buffs.GetBuffs();
  ASSERT_EQ(snapshot.size(), 1U);
  snapshot[0].name = "Changed name";
  snapshot[0].description = "Changed description";
  snapshot[0].remaining_rounds = 99;
  snapshot.clear();

  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Energy tick");
  EXPECT_EQ(info[0].description, "Adds AP when triggered.");
  EXPECT_EQ(info[0].remaining_rounds, 5);
  EXPECT_EQ(buffs.GetRemainingRounds("Energy tick"), 5);
}
