#pragma once

#include <string>
#include <vector>

#include "game/rng.h"
#include "level/level.h"

class Event : public Level {
 public:
  Event(int id, Rng& rng);

  void Enter(Player& player) override;
  void GenerateEvent();
  void DisplayChoices() const;
  void ApplyChoice(Player& player, int choice);

 private:
  Rng& rng_;
  int event_id_ = -1;
  std::string description_;
  std::vector<std::string> choices_;
};
