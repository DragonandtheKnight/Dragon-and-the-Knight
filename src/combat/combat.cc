#include "combat/combat.h"

#include <stdexcept>
#include <vector>

#include "card/effect_context.h"
#include "combat/enemy_skill_cards.h"

Combat::Combat(Player& player, const std::vector<Enemy*>& slots)
    : player_(player),
      enemies_(slots),
      current_ap_(0),
      next_ap_(0),
      next_acts_(kMaxEnemySlots, EnemySkill::kAlreadyDead) {
  if (enemies_.size() != static_cast<std::size_t>(Combat::kMaxEnemySlots)) {
    throw std::invalid_argument(
        "Combat::Combat: expected exactly 4 enemy slots");
  }
  for (Enemy* enemy : enemies_) {
    if (enemy == nullptr) {
      throw std::invalid_argument(
          "Combat::Combat: enemy slot must not be null");
    }
  }
}

int Combat::GetCurrentAp() const { return current_ap_; }

int Combat::GetNextAp() const { return next_ap_; }

Enemy* Combat::GetEnemy(int slot) const {
  if (slot < 0 || slot >= kMaxEnemySlots) {
    throw std::invalid_argument("Combat::GetEnemy: slot is out of range");
  }
  return enemies_[slot];
}

EnemySkill Combat::GetNextAct(int slot) const {
  if (slot < 0 || slot >= kMaxEnemySlots) {
    throw std::invalid_argument("Combat::GetNextAct: slot is out of range");
  }
  return next_acts_[slot];
}

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
  int cost = hand[card_index].GetCost();
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
  Card played = hand[card_index];
  player_.GetDeck().DiscardPlayedCard(card_index);
  EffectContext ctx;
  ctx.player = &player_;
  ctx.target = target;
  ctx.enemies = &enemies_;
  ctx.deck = &player_.GetDeck();
  ctx.combat = this;
  played.ApplyEffect(ctx);
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
  return GetEnemySkillCard(skill);
}
