#include "combat/combat.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "combat/buff.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
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

Card MakeCard(const std::string& name,
              std::function<void(EffectContext&)> action = {}) {
  Card card(name, "Test effect", CardRarity::kCommon, CardType::kSpecial, 1, 0);
  if (action) {
    card.AddEffect(std::make_unique<CallbackEffect>(std::move(action)));
  }
  return card;
}

class SkillCardStub : public Combat {
 public:
  SkillCardStub(Player& player, const std::vector<Enemy*>& slots, Card* skill)
      : Combat(player, slots), skill_(skill) {}

 protected:
  Card* GetSkillCard(EnemySkill) const override { return skill_; }

 private:
  Card* skill_;
};

class CombatBuffTest : public ::testing::Test {
 protected:
  void Start(const Card& hand_card = MakeCard("Hand")) {
    for (int i = 0; i < 20; ++i) {
      player_.GetDeck().AddCard(hand_card);
    }
    combat_.StartBattle();
  }

  static constexpr std::uint32_t kSeed = 42;
  Player player_{"Knight", 100, kSeed};
  Goblin enemy_{1, kSeed};
  EmptyEnemy empty_{0, kSeed};
  std::vector<Enemy*> slots_{&enemy_, &empty_, &empty_, &empty_};
  Card skill_ = MakeCard("Skill");
  SkillCardStub combat_{player_, slots_, &skill_};
};
}  // namespace

TEST_F(CombatBuffTest, GetBuffReturnsTheOwnedObjectAndAConstView) {
  static_assert(std::is_same_v<decltype(combat_.GetBuff()), Buff&>);
  const Combat& view = combat_;
  static_assert(std::is_same_v<decltype(view.GetBuff()), const Buff&>);
  EXPECT_TRUE(view.GetBuff().GetBuffs().empty());
  const Card tick = MakeCard("Tick");

  combat_.GetBuff().AddBuff(tick, 2);

  EXPECT_EQ(&combat_.GetBuff(), &view.GetBuff());
  EXPECT_EQ(view.GetBuff().GetRemainingRounds("Tick"), 2);
  Combat other(player_, slots_);
  EXPECT_TRUE(other.GetBuff().GetBuffs().empty());
}

TEST_F(CombatBuffTest, PlayedCardRegistersABuffThatTicksOncePerRound) {
  int calls = 0;
  const Card tick = MakeCard("Tick", [&](EffectContext&) { ++calls; });
  const Card potion = MakeCard("Potion", [&](EffectContext& ctx) {
    EXPECT_EQ(ctx.combat, &combat_);
    if (ctx.combat != nullptr) {
      ctx.combat->GetBuff().AddBuff(tick, 2);
    }
  });
  Start(potion);

  ASSERT_TRUE(combat_.UseCard(0, -1));
  EXPECT_EQ(calls, 0);
  EXPECT_EQ(combat_.GetBuff().GetRemainingRounds("Tick"), 2);

  combat_.EndRound();
  EXPECT_EQ(calls, 1);
  EXPECT_EQ(combat_.GetBuff().GetRemainingRounds("Tick"), 1);

  combat_.EndRound();
  EXPECT_EQ(calls, 2);
  EXPECT_TRUE(combat_.GetBuff().GetBuffs().empty());

  combat_.EndRound();
  EXPECT_EQ(calls, 2);
}

TEST_F(CombatBuffTest, EnemySkillAlsoReceivesTheCurrentCombat) {
  int calls = 0;
  skill_ = MakeCard("Skill", [&](EffectContext& ctx) {
    ++calls;
    EXPECT_EQ(ctx.combat, &combat_);
    EXPECT_EQ(ctx.player, &player_);
    EXPECT_EQ(ctx.target, &enemy_);
    EXPECT_EQ(ctx.deck, nullptr);
  });
  Start();

  combat_.EndRound();

  EXPECT_EQ(calls, 1);
}

TEST_F(CombatBuffTest, BuffReceivesContextAfterEnemySkillsBeforeApHandoff) {
  std::vector<int> order;
  skill_ = MakeCard("Skill", [&](EffectContext&) { order.push_back(1); });
  Start();
  ASSERT_TRUE(combat_.UseCard(0, -1));
  const int ap_before = combat_.GetCurrentAp();
  const Card tick = MakeCard("Tick", [&](EffectContext& ctx) {
    order.push_back(2);
    EXPECT_EQ(ctx.combat, &combat_);
    EXPECT_EQ(ctx.player, &player_);
    EXPECT_EQ(ctx.target, nullptr);
    EXPECT_EQ(ctx.deck, &player_.GetDeck());
    ASSERT_NE(ctx.enemies, nullptr);
    EXPECT_EQ(*ctx.enemies, slots_);
    EXPECT_EQ(combat_.GetCurrentAp(), ap_before);
    EXPECT_EQ(ctx.deck->GetHand().size(),
              static_cast<std::size_t>(Combat::kHandSize));
  });
  combat_.GetBuff().AddBuff(tick, 1);

  combat_.EndRound();

  EXPECT_EQ(order, (std::vector<int>{1, 2}));
  EXPECT_EQ(combat_.GetCurrentAp(), Combat::kDefaultApPerTurn);
}

TEST_F(CombatBuffTest, ClearsOldEnemyShieldBeforeSkillAndKeepsNewShield) {
  skill_ = MakeCard("Shield skill", [&](EffectContext& ctx) {
    EXPECT_EQ(ctx.target->GetShield(), 0);
    ctx.target->AddShield(7);
  });
  Start();
  enemy_.AddShield(12);

  combat_.EndRound();

  EXPECT_EQ(enemy_.GetShield(), 7);
}

TEST_F(CombatBuffTest, PlayerOldShieldBlocksAttackThenBuffShieldSurvives) {
  skill_ = MakeCard("Attack", [](EffectContext& ctx) {
    ctx.player->TakeDamage(10);
  });
  Start();
  player_.AddShield(12);
  const Card tick = MakeCard("Shield tick", [&](EffectContext& ctx) {
    EXPECT_EQ(ctx.player->GetHp(), 100);
    EXPECT_EQ(ctx.player->GetShield(), 0);
    ctx.player->AddShield(5);
  });
  combat_.GetBuff().AddBuff(tick, 1);

  combat_.EndRound();

  EXPECT_EQ(player_.GetHp(), 100);
  EXPECT_EQ(player_.GetShield(), 5);
}

TEST_F(CombatBuffTest, LethalEnemyDamageSkipsHealingBuffAndNewRoundSetup) {
  int healing_calls = 0;
  skill_ = MakeCard("Attack", [](EffectContext& ctx) {
    ctx.player->TakeDamage(10);
  });
  Start();
  player_.TakeDamage(91);
  ASSERT_EQ(player_.GetHp(), 9);
  ASSERT_TRUE(combat_.UseCard(0, -1));
  const int ap_before = combat_.GetCurrentAp();
  const auto hand_before = player_.GetDeck().GetHand().size();
  const Card tick = MakeCard("Heal tick", [&](EffectContext& ctx) {
    ++healing_calls;
    ctx.player->Heal(10);
  });
  combat_.GetBuff().AddBuff(tick, 1);

  combat_.EndRound();

  EXPECT_FALSE(player_.IsAlive());
  EXPECT_EQ(healing_calls, 0);
  EXPECT_EQ(combat_.GetBuff().GetRemainingRounds("Heal tick"), 1);
  EXPECT_EQ(combat_.GetCurrentAp(), ap_before);
  EXPECT_EQ(player_.GetDeck().GetHand().size(), hand_before);
}

TEST_F(CombatBuffTest, AlreadyDeadPlayerSkipsAllRoundActions) {
  int skill_calls = 0;
  int buff_calls = 0;
  skill_ = MakeCard("Skill", [&](EffectContext&) { ++skill_calls; });
  Start();
  enemy_.AddShield(7);
  const Card tick = MakeCard("Tick", [&](EffectContext&) { ++buff_calls; });
  combat_.GetBuff().AddBuff(tick, 1);
  player_.TakeDamage(1000);

  combat_.EndRound();

  EXPECT_EQ(skill_calls, 0);
  EXPECT_EQ(buff_calls, 0);
  EXPECT_EQ(enemy_.GetShield(), 7);
  EXPECT_EQ(combat_.GetBuff().GetRemainingRounds("Tick"), 1);
}
