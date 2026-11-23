#include "combat/enemy_skill_cards.h"

#include <memory>
#include <string>
#include <utility>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "combat/skill_cards/damage_player_effect.h"
#include "combat/skill_cards/heal_enemies_effect.h"
#include "combat/skill_cards/heal_target_effect.h"
#include "combat/skill_cards/shield_max_hp_enemy_effect.h"
#include "combat/skill_cards/shield_target_effect.h"
#include "enemy/enemy_skill.h"

static Card MakeCard(const std::string& name, const std::string& description,
                     CardRarity rarity, CardType type, int value,
                     std::unique_ptr<CardEffect> effect) {
  Card card(name, description, rarity, type, 0, value);
  card.AddEffect(std::move(effect));
  return card;
}

Card* GetEnemySkillCard(EnemySkill skill) {
  switch (skill) {
      // ------------------------------slime---------------------------------
    case EnemySkill::kSlimeSlapLv1: {
      static Card card = MakeCard("Slime slap", "Deals 3 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 3,
                                  std::make_unique<DamagePlayerEffect>(3));
      return &card;
    }
    case EnemySkill::kSlimeHealLv1: {
      static Card card = MakeCard(
          "Slime heal", "Heals the casting slime for 5.", CardRarity::kCommon,
          CardType::kHeal, 5, std::make_unique<HealTargetEffect>(5));
      return &card;
    }
    case EnemySkill::kSlimeSlapLv2: {
      static Card card = MakeCard("Slime slap", "Deals 5 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 5,
                                  std::make_unique<DamagePlayerEffect>(5));
      return &card;
    }
    case EnemySkill::kSlimeHealLv2: {
      static Card card = MakeCard(
          "Slime heal", "Heals the casting slime for 8.", CardRarity::kCommon,
          CardType::kHeal, 8, std::make_unique<HealTargetEffect>(8));
      return &card;
    }
    case EnemySkill::kSlimeSlapLv3: {
      static Card card = MakeCard("Slime slap", "Deals 8 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 8,
                                  std::make_unique<DamagePlayerEffect>(8));
      return &card;
    }
    case EnemySkill::kSlimeHealLv3: {
      static Card card = MakeCard(
          "Slime heal", "Heals the casting slime for 12.", CardRarity::kCommon,
          CardType::kHeal, 12, std::make_unique<HealTargetEffect>(12));
      return &card;
    }
      // ------------------------------goblin---------------------------------
    case EnemySkill::kGoblinHitLv1: {
      static Card card = MakeCard("Goblin hit", "Deals 5 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 5,
                                  std::make_unique<DamagePlayerEffect>(5));
      return &card;
    }
    case EnemySkill::kGoblinHitLv2: {
      static Card card = MakeCard("Goblin hit", "Deals 8 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 8,
                                  std::make_unique<DamagePlayerEffect>(8));
      return &card;
    }
    case EnemySkill::kGoblinHardSlamLv2: {
      static Card card =
          MakeCard("Goblin hard slam", "Deals 12 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 12,
                   std::make_unique<DamagePlayerEffect>(12));
      return &card;
    }
    case EnemySkill::kGoblinHitLv3: {
      static Card card = MakeCard(
          "Goblin hit", "Deals 12 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 12, std::make_unique<DamagePlayerEffect>(12));
      return &card;
    }
    case EnemySkill::kGoblinHardSlamLv3: {
      static Card card =
          MakeCard("Goblin hard slam", "Deals 18 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 18,
                   std::make_unique<DamagePlayerEffect>(18));
      return &card;
    }
    case EnemySkill::kGoblinDashLv3: {
      static Card card =
          MakeCard("Goblin dash",
                   "Gives the caster 15 shield, next move would deal 22 dmg",
                   CardRarity::kCommon, CardType::kDefense, 15,
                   std::make_unique<ShieldTargetEffect>(15));
      return &card;
    }
    case EnemySkill::kGoblinDashAttackLv3: {
      static Card card =
          MakeCard("Goblin dash attack", "Deals 22 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 22,
                   std::make_unique<DamagePlayerEffect>(22));
      return &card;
    }
      // ------------------------------slime---------------------------------
    case EnemySkill::kShamanCurseLv1: {
      static Card card = MakeCard(
          "Shaman curse", "Deals 5 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 5, std::make_unique<DamagePlayerEffect>(5));
      return &card;
    }
    case EnemySkill::kShamanHealAllLv1: {
      static Card card = MakeCard(
          "Shaman heal all", "Heals every enemy for 10% of its max health.",
          CardRarity::kCommon, CardType::kHeal, 10,
          std::make_unique<HealEnemiesEffect>(10));
      return &card;
    }
    case EnemySkill::kShamanCurseLv2: {
      static Card card = MakeCard(
          "Shaman curse", "Deals 8 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 8, std::make_unique<DamagePlayerEffect>(8));
      return &card;
    }
    case EnemySkill::kShamanHealAllLv2: {
      static Card card = MakeCard(
          "Shaman heal all", "Heals every enemy for 15% of its max health.",
          CardRarity::kCommon, CardType::kHeal, 15,
          std::make_unique<HealEnemiesEffect>(15));
      return &card;
    }
    case EnemySkill::kShamanGiveShieldLv2: {
      static Card card = MakeCard("Shaman give shield",
                                  "Gives the healthiest enemy 12 shield.",
                                  CardRarity::kCommon, CardType::kHeal, 12,
                                  std::make_unique<ShieldMaxHpEnemyEffect>(12));
      return &card;
    }
    case EnemySkill::kShamanCurseLv3: {
      static Card card = MakeCard(
          "Shaman curse", "Deals 12 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 12, std::make_unique<DamagePlayerEffect>(12));
      return &card;
    }
    case EnemySkill::kShamanHealAllLv3: {
      static Card card = MakeCard(
          "Shaman heal all", "Heals every enemy for 20% of its max health.",
          CardRarity::kCommon, CardType::kHeal, 20,
          std::make_unique<HealEnemiesEffect>(20));
      return &card;
    }
    case EnemySkill::kShamanGiveShieldLv3: {
      static Card card = MakeCard("Shaman give shield",
                                  "Gives the healthiest enemy 16 shield.",
                                  CardRarity::kCommon, CardType::kHeal, 16,
                                  std::make_unique<ShieldMaxHpEnemyEffect>(16));
      return &card;
    }
    case EnemySkill::kShamanHealSelfLv3: {
      static Card card =
          MakeCard("Shaman heal self", "Heals the casting shaman for 20.",
                   CardRarity::kCommon, CardType::kHeal, 20,
                   std::make_unique<HealTargetEffect>(20));
      return &card;
    }
    // ------------------------------giant---------------------------------
    case EnemySkill::kGiantSlamLv1: {
      static Card card = MakeCard("Giant slam", "Deals 8 damage to the player.",
                                  CardRarity::kCommon, CardType::kAttack, 8,
                                  std::make_unique<DamagePlayerEffect>(8));
      return &card;
    }
    case EnemySkill::kGiantDefendLv1: {
      static Card card = MakeCard("Giant defend", "Gives the caster 12 shield.",
                                  CardRarity::kCommon, CardType::kDefense, 12,
                                  std::make_unique<ShieldTargetEffect>(12));
      return &card;
    }
    case EnemySkill::kGiantSlamLv2: {
      static Card card = MakeCard(
          "Giant slam", "Deals 12 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 12, std::make_unique<DamagePlayerEffect>(12));
      return &card;
    }
    case EnemySkill::kGiantDefendLv2: {
      static Card card = MakeCard("Giant defend", "Gives the caster 16 shield.",
                                  CardRarity::kCommon, CardType::kDefense, 16,
                                  std::make_unique<ShieldTargetEffect>(16));
      return &card;
    }
    case EnemySkill::kGiantChargeLv2: {
      static Card card = MakeCard(
          "Giant charge", "Deals 16 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 16, std::make_unique<DamagePlayerEffect>(16));
      return &card;
    }
    case EnemySkill::kGiantSlamLv3: {
      static Card card = MakeCard(
          "Giant slam", "Deals 16 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 16, std::make_unique<DamagePlayerEffect>(16));
      return &card;
    }
    case EnemySkill::kGiantDefendLv3: {
      static Card card = MakeCard("Giant defend", "Gives the caster 20 shield.",
                                  CardRarity::kCommon, CardType::kDefense, 20,
                                  std::make_unique<ShieldTargetEffect>(20));
      return &card;
    }
    case EnemySkill::kGiantSuperChargeLv3: {
      static Card card("Giant super charge",
                       "Charges up: next move deals 34 damage.",
                       CardRarity::kCommon, CardType::kSpecial, 0, 0);
      return &card;
    }
    case EnemySkill::kGiantSuperChargeHitLv3: {
      static Card card =
          MakeCard("Giant super charge hit", "Deals 34 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 34,
                   std::make_unique<DamagePlayerEffect>(34));
      return &card;
    }
      // ------------------------------dragon---------------------------------
    case EnemySkill::kDragonClawLv1: {
      static Card card = MakeCard(
          "Dragon claw", "Deals 10 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 10, std::make_unique<DamagePlayerEffect>(10));
      return &card;
    }
    case EnemySkill::kDragonBreathLv1: {
      static Card card =
          MakeCard("Dragon breath", "Deals 14 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 14,
                   std::make_unique<DamagePlayerEffect>(14));
      return &card;
    }
    case EnemySkill::kDragonClawLv2: {
      static Card card = MakeCard(
          "Dragon claw", "Deals 14 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 14, std::make_unique<DamagePlayerEffect>(14));
      return &card;
    }
    case EnemySkill::kDragonBreathLv2: {
      static Card card =
          MakeCard("Dragon breath", "Deals 18 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 18,
                   std::make_unique<DamagePlayerEffect>(18));
      return &card;
    }
    case EnemySkill::kDragonFlyLv2: {
      static Card card = MakeCard(
          "Dragon fly",
          "Flies up and gets a 999 shield and prepare for a drop slam.",
          CardRarity::kCommon, CardType::kDefense, 999,
          std::make_unique<ShieldTargetEffect>(999));
      return &card;
    }
    case EnemySkill::kDragonDropSlamLv2: {
      static Card card =
          MakeCard("Dragon drop slam", "Deals 25 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 25,
                   std::make_unique<DamagePlayerEffect>(25));
      return &card;
    }
    case EnemySkill::kDragonClawLv3: {
      static Card card = MakeCard(
          "Dragon claw", "Deals 18 damage to the player.", CardRarity::kCommon,
          CardType::kAttack, 18, std::make_unique<DamagePlayerEffect>(18));
      return &card;
    }
    case EnemySkill::kDragonBreathLv3: {
      static Card card =
          MakeCard("Dragon breath", "Deals 24 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 24,
                   std::make_unique<DamagePlayerEffect>(24));
      return &card;
    }
    case EnemySkill::kDragonFlyLv3: {
      static Card card = MakeCard(
          "Dragon fly",
          "Flies up and gets a 999 shield and prepare for a drop slam.",
          CardRarity::kCommon, CardType::kDefense, 999,
          std::make_unique<ShieldTargetEffect>(999));
      return &card;
    }
    case EnemySkill::kDragonDropSlamLv3: {
      static Card card =
          MakeCard("Dragon drop slam", "Deals 35 damage to the player.",
                   CardRarity::kCommon, CardType::kAttack, 35,
                   std::make_unique<DamagePlayerEffect>(35));
      return &card;
    }
    case EnemySkill::kDragonRoarLv3: {
      static Card card =
          MakeCard("Dragon roar", "Heals the casting dragon for 75.",
                   CardRarity::kCommon, CardType::kHeal, 75,
                   std::make_unique<HealTargetEffect>(75));
      return &card;
    }
    default:
      return nullptr;
  }
}
