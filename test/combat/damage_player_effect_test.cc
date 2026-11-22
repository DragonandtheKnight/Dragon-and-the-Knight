#include "combat/skill_cards/damage_player_effect.h"

#include <cstdint>
#include <memory>

#include "card/card.h"
#include "card/card_effect.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/effect_context.h"
#include "gtest/gtest.h"
#include "player/player.h"

namespace {
constexpr std::uint32_t kSeed = 42;
}  // namespace

TEST(DamagePlayerEffectTest, TakesTheStoredAmountFromThePlayer) {
  Player player("Knight", 100, kSeed);
  const DamagePlayerEffect effect(3);

  EffectContext context;
  context.player = &player;
  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 97);
}

TEST(DamagePlayerEffectTest, KeepsTheAmountAcrossTwoUses) {
  Player player("Knight", 100, kSeed);
  const DamagePlayerEffect effect(8);

  EffectContext context;
  context.player = &player;
  effect.Apply(context);
  effect.Apply(context);

  EXPECT_EQ(player.GetHp(), 84);
}

TEST(DamagePlayerEffectTest, DoesNothingWhenTheContextHasNoPlayer) {
  const DamagePlayerEffect effect(3);

  EffectContext context;
  context.player = nullptr;
  effect.Apply(context);
}

TEST(DamagePlayerEffectTest, CloneCarriesTheAmount) {
  const DamagePlayerEffect effect(8);
  const std::unique_ptr<CardEffect> copy = effect.Clone();

  Player player("Knight", 100, kSeed);
  EffectContext context;
  context.player = &player;
  ASSERT_NE(copy, nullptr);
  copy->Apply(context);

  EXPECT_EQ(player.GetHp(), 92);
}

TEST(DamagePlayerEffectTest, RunsWhenTheCardCarriesIt) {
  Player player("Knight", 100, kSeed);
  Card card("Goblin hit", "Hits the player once", CardRarity::kCommon,
            CardType::kAttack, 1, 5);
  card.AddEffect(std::make_unique<DamagePlayerEffect>(5));

  EffectContext context;
  context.player = &player;
  card.ApplyEffect(context);

  EXPECT_EQ(player.GetHp(), 95);
}
