#include "card/card_factory.h"

#include <memory>

#include "card/block_effect.h"
#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "card/damage_effect.h"
#include "card/heal_effect.h"
#include "card/target_type.h"

Card CardFactory::CreateStrike() {
  constexpr int kDamage = 5;
  Card card("Strike", "Deal 5 damage to one enemy.", CardRarity::kCommon,
            CardType::kAttack, 1, kDamage);
  card.AddEffect(
      std::make_unique<DamageEffect>(kDamage, TargetType::kSingleEnemy));
  return card;
}

Card CardFactory::CreateDefend() {
  constexpr int kShield = 5;
  Card card("Defend", "Gain 5 shield.", CardRarity::kCommon, CardType::kDefense,
            1, kShield);
  card.AddEffect(std::make_unique<BlockEffect>(kShield, TargetType::kSelf));
  return card;
}

Card CardFactory::CreateLifeDrain() {
  constexpr int kDamage = 5;
  Card card("Life Drain", "Deal 5 damage to one enemy, then heal 2 HP.",
            CardRarity::kRare, CardType::kAttack, 2, kDamage);
  card.AddEffect(
      std::make_unique<DamageEffect>(kDamage, TargetType::kSingleEnemy));
  card.AddEffect(std::make_unique<HealEffect>(2, TargetType::kSelf));
  return card;
}

Card CardFactory::CreateWhirlwind() {
  constexpr int kDamage = 3;
  Card card("Whirlwind", "Deal 3 damage to all enemies.", CardRarity::kRare,
            CardType::kAttack, 1, kDamage);
  card.AddEffect(
      std::make_unique<DamageEffect>(kDamage, TargetType::kAllEnemies));
  return card;
}
