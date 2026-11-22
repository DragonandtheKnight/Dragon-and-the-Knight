#include "combat/buff.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "gtest/gtest.h"

namespace {
class CallbackEffect : public CardEffect {
 public:
  explicit CallbackEffect(std::function<void(EffectContext&)> action)
      : action_(std::move(action)) {}

  void Apply(EffectContext& ctx) const override { action_(ctx); }

  std::unique_ptr<CardEffect> Clone() const override {
    return std::make_unique<CallbackEffect>(action_);
  }

 private:
  std::function<void(EffectContext&)> action_;
};

Card MakeBuffCard(const std::string& name,
                  std::function<void(EffectContext&)> action) {
  Card card(name, "Test effect", CardRarity::kCommon, CardType::kSpecial, 0, 0);
  card.AddEffect(std::make_unique<CallbackEffect>(std::move(action)));
  return card;
}
}  // namespace

TEST(BuffApplyTest, EmptyBuffDoesNothing) {
  Buff buffs;
  EffectContext ctx;

  EXPECT_NO_THROW(buffs.ApplyBuffs(ctx));

  EXPECT_TRUE(buffs.GetBuffs().empty());
}

TEST(BuffApplyTest, ConsumesOneCardPerGroupInFifoOrderAndStopsAtExpiry) {
  std::vector<int> calls;
  Buff buffs;
  EffectContext ctx;
  const Card first = MakeBuffCard("Tick", [&](EffectContext&) {
    calls.push_back(1);
  });
  const Card later = MakeBuffCard("Tick", [&](EffectContext&) {
    calls.push_back(2);
  });
  buffs.AddBuff(first, 2);
  buffs.AddBuff(later, 1);

  buffs.ApplyBuffs(ctx);
  EXPECT_EQ(calls, (std::vector<int>{1}));
  EXPECT_EQ(buffs.GetRemainingRounds("Tick"), 2);

  buffs.ApplyBuffs(ctx);
  EXPECT_EQ(calls, (std::vector<int>{1, 1}));
  EXPECT_EQ(buffs.GetRemainingRounds("Tick"), 1);

  buffs.ApplyBuffs(ctx);
  EXPECT_EQ(calls, (std::vector<int>{1, 1, 2}));
  EXPECT_EQ(buffs.GetRemainingRounds("Tick"), 0);
  EXPECT_TRUE(buffs.GetBuffs().empty());

  buffs.ApplyBuffs(ctx);
  EXPECT_EQ(calls, (std::vector<int>{1, 1, 2}));
}

TEST(BuffApplyTest, ExpiringGroupsDoNotSkipTheirNeighbours) {
  std::vector<int> calls;
  Buff buffs;
  EffectContext ctx;
  const Card first = MakeBuffCard("First", [&](EffectContext&) {
    calls.push_back(1);
  });
  const Card second = MakeBuffCard("Second", [&](EffectContext&) {
    calls.push_back(2);
  });
  const Card third = MakeBuffCard("Third", [&](EffectContext&) {
    calls.push_back(3);
  });
  buffs.AddBuff(first, 1);
  buffs.AddBuff(second, 2);
  buffs.AddBuff(third, 1);

  buffs.ApplyBuffs(ctx);

  EXPECT_EQ(calls, (std::vector<int>{1, 2, 3}));
  const std::vector<BuffInfo> info = buffs.GetBuffs();
  ASSERT_EQ(info.size(), 1U);
  EXPECT_EQ(info[0].name, "Second");
  EXPECT_EQ(info[0].remaining_rounds, 1);

  buffs.ApplyBuffs(ctx);
  EXPECT_EQ(calls, (std::vector<int>{1, 2, 3, 2}));
  EXPECT_TRUE(buffs.GetBuffs().empty());
}

TEST(BuffApplyTest, ExecutesAnOwnedCopyWithTheExactSuppliedContext) {
  EffectContext* seen = nullptr;
  Buff buffs;
  EffectContext ctx;
  {
    const Card source = MakeBuffCard("Tick", [&](EffectContext& received) {
      seen = &received;
    });
    buffs.AddBuff(source, 1);
  }

  buffs.ApplyBuffs(ctx);

  EXPECT_EQ(seen, &ctx);
  EXPECT_TRUE(buffs.GetBuffs().empty());
}
