// Opt-in tests for per-effect targeting and Combat validation.

#include <memory>
#include <stdexcept>
#include <vector>

#include "card/block_effect.h"
#include "card/card.h"
#include "card/damage_effect.h"
#include "card/deck.h"
#include "card/effect_context.h"
#include "card/heal_effect.h"
#include "card/target_type.h"
#include "combat/combat.h"
#include "enemy/empty_enemy.h"
#include "enemy/goblin.h"
#include "gtest/gtest.h"
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
    p.TakeDamage(10);
    Goblin e(1, 42);
    Card card = MixedCard();
    std::vector<EffectContext> c(count);
    if (count > 0) {
      c[0].caster_player = &p;
      c[0].target_enemy = &e;
    }
    if (count > 1) {
      c[1].caster_player = &p;
      c[1].target_player = &p;
    }
    EXPECT_THROW(card.ApplyEffect(c), std::invalid_argument);
    EXPECT_EQ(e.GetHp(), 30);
    EXPECT_EQ(p.GetHp(), 20);
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
// These cases need the Combat owner to wire in shared targeting.
TEST(PendingCombatIntegration,
     DISABLED_InvalidLaterTargetLeavesEffectsApAndHandUnchanged) {
  Player player("Knight", 30, 42);
  player.TakeDamage(10);
  Goblin enemy(1, 42);
  Goblin dead(1, 43);
  dead.TakeDamage(100);
  EmptyEnemy empty(0, 44);
  Combat combat(player, {&enemy, &dead, &empty, &empty});

  Card card("Heal and strike", "", CardRarity::kCommon, CardType::kAttack, 2,
            0);
  card.AddEffect(std::make_unique<HealEffect>(5, TargetType::kSelf));
  card.AddEffect(std::make_unique<DamageEffect>(8, TargetType::kSingleEnemy));
  player.GetDeck().AddCard(card);
  combat.StartBattle();
  ASSERT_EQ(player.GetDeck().GetHand().size(), 1U);
  int ap = combat.GetCurrentAp();

  for (int slot : {-1, 1, 2}) {
    EXPECT_FALSE(combat.UseCard(0, slot));
    EXPECT_EQ(player.GetHp(), 20);
    EXPECT_EQ(enemy.GetHp(), 30);
    EXPECT_EQ(combat.GetCurrentAp(), ap);
    EXPECT_EQ(player.GetDeck().GetHand().size(), 1U);
  }
  // The draw pile is empty. A rejected card cannot be in the discard pile.
  player.GetDeck().Draw(1);
  ASSERT_EQ(player.GetDeck().GetHand().size(), 1U);

  ASSERT_TRUE(combat.UseCard(0, 0));
  EXPECT_EQ(player.GetHp(), 25);
  EXPECT_EQ(enemy.GetHp(), 22);
  EXPECT_EQ(combat.GetCurrentAp(), ap - 2);
  EXPECT_TRUE(player.GetDeck().GetHand().empty());
}

TEST(PlannedMulti, DirectCardApplicationDoesNotSpendCombatAp) {
  Player p("P", 30, 42);
  p.TakeDamage(10);
  Goblin e(1, 42);
  EmptyEnemy empty(0, 43);
  Combat combat(p, {&e, &empty, &empty, &empty});
  combat.StartBattle();
  Card card = MixedCard();
  std::vector<EffectContext> c(2);
  c[0].caster_player = &p;
  c[0].target_enemy = &e;
  c[0].combat = &combat;
  c[1].caster_player = &p;
  c[1].target_player = &p;
  c[1].combat = &combat;
  int ap = combat.GetCurrentAp();

  card.ApplyEffect(c);

  EXPECT_EQ(combat.GetCurrentAp(), ap);
  EXPECT_EQ(e.GetHp(), 22);
  EXPECT_EQ(p.GetHp(), 25);
}

namespace {

class SharedSkillCombat : public Combat {
 public:
  SharedSkillCombat(Player& player, const std::vector<Enemy*>& slots,
                    const Card& skill)
      : Combat(player, slots), skill_(skill) {}

 private:
  const Card* GetSkillCard(EnemySkill) const override { return &skill_; }

  const Card& skill_;
};

}  // namespace

TEST(PendingCombatIntegration,
     DISABLED_EnemySkillUsesSharedEffectsWithTheCorrectRecipients) {
  Player player("Knight", 30, 42);
  player.AddShield(3);
  Goblin caster(1, 42);
  caster.TakeDamage(10);
  EmptyEnemy empty(0, 43);
  Card skill("Strike and guard", "", CardRarity::kCommon, CardType::kAttack, 0,
             0);
  skill.AddEffect(std::make_unique<DamageEffect>(8, TargetType::kSingleEnemy));
  skill.AddEffect(std::make_unique<HealEffect>(5, TargetType::kSelf));
  skill.AddEffect(std::make_unique<BlockEffect>(6, TargetType::kSelf));
  SharedSkillCombat combat(player, {&caster, &empty, &empty, &empty}, skill);
  combat.StartBattle();

  combat.EndRound();

  EXPECT_EQ(player.GetHp(), 25);
  EXPECT_EQ(player.GetShield(), 0);
  EXPECT_EQ(caster.GetHp(), 25);
  EXPECT_EQ(caster.GetShield(), 6);
  EXPECT_EQ(empty.GetShield(), 0);
}

TEST(PendingCombatIntegration,
     DISABLED_CombatResolvesAllEnemiesWithoutASlotSelection) {
  Player player("Knight", 30, 42);
  Goblin first(1, 42);
  Goblin second(1, 43);
  Goblin dead(1, 44);
  dead.TakeDamage(100);
  EmptyEnemy empty(0, 45);
  Combat combat(player, {&first, &dead, &empty, &second});

  Card card("Sweep", "", CardRarity::kCommon, CardType::kAttack, 2, 0);
  card.AddEffect(std::make_unique<DamageEffect>(8, TargetType::kAllEnemies));
  player.GetDeck().AddCard(card);
  combat.StartBattle();
  int ap = combat.GetCurrentAp();

  ASSERT_TRUE(combat.UseCard(0, -1));
  EXPECT_EQ(first.GetHp(), 22);
  EXPECT_EQ(second.GetHp(), 22);
  EXPECT_EQ(dead.GetHp(), 0);
  EXPECT_EQ(empty.GetHp(), 0);
  EXPECT_EQ(player.GetHp(), 30);
  EXPECT_EQ(combat.GetCurrentAp(), ap - 2);
  EXPECT_TRUE(player.GetDeck().GetHand().empty());
}
