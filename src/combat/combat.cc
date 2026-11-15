#include "combat/combat.h"

#include <vector>

#include "card/effect_context.h"

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
  card.ApplyEffect(ctx);
  player_.GetDeck().DiscardPlayedCard(card_index);
  return true;
}
