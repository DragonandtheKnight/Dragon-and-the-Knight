#include "combat/combat.h"

#include <memory>
#include <string>
#include <vector>

#include "card/effect_context.h"
#include "combat/skill_cards/damage_player_effect.h"
#include "combat/skill_cards/heal_enemies_effect.h"
#include "combat/skill_cards/heal_target_effect.h"
#include "combat/skill_cards/shield_max_hp_enemy_effect.h"
#include "combat/skill_cards/shield_target_effect.h"

static Card MakeCard(const std::string& name, const std::string& description,
                     CardRarity rarity, CardType type, int value,
                     std::unique_ptr<CardEffect> effect) {
  Card card(name, description, rarity, type, 0, value);
  card.AddEffect(std::move(effect));
  return card;
}

const int Combat::kDefaultApPerTurn;

Combat::Combat(Player& player, const std::vector<Enemy*>& slots)
    : player_(player),
      enemies_(slots),
      current_ap_(0),
      next_ap_(0),
      next_acts_(kMaxEnemySlots, EnemySkill::kAlreadyDead) {}

int Combat::GetCurrentAp() const { return current_ap_; }

int Combat::GetNextAp() const { return next_ap_; }

Enemy* Combat::GetEnemy(int slot) const { return enemies_[slot]; }

EnemySkill Combat::GetNextAct(int slot) const { return next_acts_[slot]; }

bool Combat::IsOver() const {
  if (!player_.IsAlive()) {
    return true;
  }
  for (int slot = 0; slot < kMaxEnemySlots; ++slot) {
    if (enemies_[slot]->IsAlive()) {
      return false;
    }
  }
  return true;
}

void Combat::StartBattle() {
  Deck& deck = player_.GetDeck();
  deck.StartBattle();
  deck.Shuffle();
  deck.Draw(kHandSize);
  int slot;
  for (slot = 0; slot < kMaxEnemySlots; slot++) {
    next_acts_[slot] = enemies_[slot]->CalculateMove();
  }
  current_ap_ = kDefaultApPerTurn;
  next_ap_ = kDefaultApPerTurn;
}

bool Combat::UseCard(int card_index, int target_slot) {
  const std::vector<Card>& hand = player_.GetDeck().GetHand();
  if (target_slot < -1 || target_slot >= kMaxEnemySlots) {
    return false;
  }
  if (card_index < 0 || static_cast<std::size_t>(card_index) >= hand.size()) {
    return false;
  }
  const Card& card = hand[card_index];
  int cost = card.GetCost();
  if (current_ap_ < cost) {
    return false;
  }
  Enemy* target = nullptr;
  if (target_slot == -1) {
    target = nullptr;
  } else if (target_slot >= 0 && target_slot < kMaxEnemySlots) {
    target = enemies_[target_slot];
    if (target->IsEmpty()) {
      return false;
    }
    if (!target->IsAlive()) {
      return false;
    }
  }
  current_ap_ -= cost;
  EffectContext ctx;
  ctx.player = &player_;
  ctx.target = target;
  ctx.enemies = &enemies_;
  ctx.deck = &player_.GetDeck();
  ctx.combat = this;
  card.ApplyEffect(ctx);
  player_.GetDeck().DiscardPlayedCard(card_index);
  return true;
}

void Combat::ResolveOneSkill(EnemySkill skill, int slot) {
  Card* card = GetSkillCard(skill);
  if (card == nullptr) {
    return;
  }
  Enemy* target = enemies_[slot];
  EffectContext ctx;
  ctx.player = &player_;
  ctx.target = target;
  ctx.enemies = &enemies_;
  ctx.deck = nullptr;
  ctx.combat = this;
  card->ApplyEffect(ctx);
}

void Combat::ResolveEnemySkills() {
  int slot;
  for (slot = 0; slot < kMaxEnemySlots; slot++) {
    if (enemies_[slot]->IsAlive()) {
      ResolveOneSkill(next_acts_[slot], slot);
    }
  }
}

void Combat::EndRound() {
  if (!player_.IsAlive()) {
    return;
  }
  int slot;
  for (slot = 0; slot < kMaxEnemySlots; slot++) {
    enemies_[slot]->ClearShield();
  }
  ResolveEnemySkills();
  if (!player_.IsAlive()) {
    return;
  }
  player_.ResetShield();
  player_.GetDeck().DiscardHand();
  player_.GetDeck().Draw(kHandSize);
  EffectContext ctx;
  ctx.player = &player_;
  ctx.target = nullptr;
  ctx.enemies = &enemies_;
  ctx.deck = &player_.GetDeck();
  ctx.combat = this;
  buff_.ApplyBuffs(ctx);
  current_ap_ = next_ap_;
  next_ap_ = kDefaultApPerTurn;
  for (slot = 0; slot < kMaxEnemySlots; slot++) {
    next_acts_[slot] = enemies_[slot]->CalculateMove();
  }
}

Buff& Combat::GetBuff() { return buff_; }

const Buff& Combat::GetBuff() const { return buff_; }

Card* Combat::GetSkillCard(EnemySkill skill) const {
  switch (skill) {
      //------------------------------slime---------------------------------
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
      //------------------------------goblin---------------------------------
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
      //------------------------------slime---------------------------------
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
    default:
      return nullptr;
  }
}
