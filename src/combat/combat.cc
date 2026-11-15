#include "combat/combat.h"

#include <vector>

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
