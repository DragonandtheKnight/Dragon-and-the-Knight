#include "level/entertainment.h"

#include <algorithm>
#include <iostream>

#include "level/level_type.h"
#include "player/player.h"

Entertainment::Entertainment(int id, int coin_cost, Rng& rng)
    : Level(id, LevelType::kEntertainment, false),
      coin_cost_(std::max(0, coin_cost)),
      rng_(rng) {}

void Entertainment::Enter(Player& player) {
  if (!player.SpendGold(coin_cost_)) {
    std::cout << "Not enough gold to play.\n";
    return;
  }

  const int roll = Roll();
  reward_ = CalculateReward(roll);
  player.GainGold(reward_);
  std::cout << "Rolled " << roll << ", won " << reward_ << " gold.\n";
}

int Entertainment::Roll() { return rng_.UniformInt(1, 6); }

int Entertainment::CalculateReward(int roll) const {
  switch (roll) {
    case 1:
    case 2:
      return 0;
    case 3:
    case 4:
      return coin_cost_ * 2;
    case 5:
      return coin_cost_ * 3;
    case 6:
      return coin_cost_ * 5;
    default:
      return 0;
  }
}
