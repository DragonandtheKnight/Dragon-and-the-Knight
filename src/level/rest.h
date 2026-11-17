#pragma once

#include "level/level.h"

class Rest : public Level {
 public:
  Rest(int id, int heal_amount);

  void Enter(Player& player) override;
  void HealPlayer(Player& player);

 private:
  int heal_amount_;
};
