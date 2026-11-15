#pragma once

#include "game/rng.h"
#include "level/level.h"

class Entertainment : public Level {
 public:
  Entertainment(int id, int coin_cost, Rng& rng);

  void Enter(Player& player) override;
  int Roll();
  int CalculateReward(int roll) const;

 private:
  int coin_cost_;
  int reward_ = 0;
  Rng& rng_;
};
