// Planned tests (RED), disabled by default; APIs are not implemented.
// Proposals: constructors accept TargetType;
// Battle::TryPlay validates targets/AP.
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "battle/battle.h"
#include "card/block_effect.h"
#include "card/card.h"
#include "card/damage_effect.h"
#include "card/effect_context.h"
#include "card/heal_effect.h"
#include "card/target_type.h"
#include "enemy/goblin.h"
#include "player/player.h"
namespace {
Card MixedCard() {
  Card card("Fixture", "", CardRarity::kCommon, CardType::kAttack, 2, 0);
  card.AddEffect(std::make_unique<DamageEffect>(8, TargetType::kSingleEnemy));
  card.AddEffect(std::make_unique<HealEffect>(5, TargetType::kSelf));
  return card;
}
}  // namespace
TEST(PlannedMulti, EachEffectUsesItsOwnContext) {
  Player p("P", 30, 42);
  p.TakeDamage(10);
  Goblin e(1, 42);
  Card card = MixedCard();
  std::vector<EffectContext> c(2);
  c[0].caster_player = &p;
  c[0].target_enemy = &e;
  c[1].caster_player = &p;
  c[1].target_player = &p;
  card.ApplyEffect(c);
  EXPECT_EQ(e.GetHp(), 22);
  EXPECT_EQ(p.GetHp(), 25);
  EXPECT_EQ(card.GetCost(), 2);
  EXPECT_EQ(
      card.GetEffectTargetTypes(),
      (std::vector<TargetType>{TargetType::kSingleEnemy, TargetType::kSelf}));
}
TEST(PlannedMulti, ContextCountMismatchExecutesNothing) {
  for (int count : {0, 1, 3}) {
    Player p("P", 30, 42);
    Goblin e(1, 42);
    Card card = MixedCard();
    std::vector<EffectContext> c(count);
    for (auto& x : c) {
      x.caster_player = &p;
      x.target_enemy = &e;
      x.target_player = &p;
    }
    EXPECT_THROW(card.ApplyEffect(c), std::invalid_argument);
    EXPECT_EQ(e.GetHp(), 30);
    EXPECT_EQ(p.GetHp(), 30);
  }
}
TEST(PlannedMulti, LethalEffectSkipsDeadTargetButContinuesOtherTargets) {
  Player p("P", 30, 42);
  p.TakeDamage(10);
  Goblin e(1, 42);
  Card card("Fixture", "", CardRarity::kCommon, CardType::kAttack, 2, 0);
  card.AddEffect(std::make_unique<DamageEffect>(100, TargetType::kSingleEnemy));
  card.AddEffect(std::make_unique<BlockEffect>(9, TargetType::kSingleEnemy));
  card.AddEffect(std::make_unique<HealEffect>(5, TargetType::kSelf));
  std::vector<EffectContext> c(3);
  for (auto& x : c) x.caster_player = &p;
  c[0].target_enemy = &e;
  c[1].target_enemy = &e;
  c[2].target_player = &p;
  card.ApplyEffect(c);
  EXPECT_EQ(e.GetHp(), 0);
  EXPECT_EQ(e.GetShield(), 0);
  EXPECT_EQ(p.GetHp(), 25);
}
TEST(PlannedMulti, InvalidLaterTargetPreventsAnyExecutionAndApSpend) {
  Player p("P", 30, 42);
  Goblin e(1, 42);
  Battle battle(p, {&e});
  Card card = MixedCard();
  std::vector<EffectContext> c(2);
  c[0].caster_player = &p;
  c[0].target_enemy = &e;
  c[1].caster_player = &p;  // missing self target: invalid selection
  int ap = battle.GetCurrentAp();
  EXPECT_FALSE(battle.TryPlay(card, c));
  EXPECT_EQ(e.GetHp(), 30);
  EXPECT_EQ(battle.GetCurrentAp(), ap);
}
TEST(PlannedMulti, DirectCardApplicationDoesNotSpendBattleAp) {
  Player p("P", 30, 42);
  Goblin e(1, 42);
  Battle battle(p, {&e});
  Card card = MixedCard();
  std::vector<EffectContext> c(2);
  c[0].caster_player = &p;
  c[0].target_enemy = &e;
  c[1].caster_player = &p;
  c[1].target_player = &p;
  int ap = battle.GetCurrentAp();
  card.ApplyEffect(c);
  EXPECT_EQ(battle.GetCurrentAp(), ap);
}
