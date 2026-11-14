#include "level/event.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "level/level_type.h"
#include "player/player.h"

Event::Event(int id, Rng& rng)
    : Level(id, LevelType::kEvent, false), rng_(rng) {}

void Event::Enter(Player& player) {
  (void)player;
  GenerateEvent();
  DisplayChoices();
}

void Event::GenerateEvent() {
  event_id_ = rng_.UniformInt(0, 2);
  switch (event_id_) {
    case 0:
      description_ = "You meet a wandering healer.";
      choices_ = {"Heal 20 HP", "Receive 20 gold"};
      break;
    case 1:
      description_ = "You find an ancient shrine.";
      choices_ = {"Gain 10 max HP", "Gain 10 block"};
      break;
    default:
      description_ = "You encounter a mysterious merchant.";
      choices_ = {"Receive 30 gold", "Pay 10 gold for 1 max energy"};
      break;
  }
}

void Event::DisplayChoices() const {
  std::cout << description_ << '\n';
  for (std::size_t i = 0; i < choices_.size(); ++i) {
    std::cout << i << ": " << choices_[i] << '\n';
  }
}

void Event::ApplyChoice(Player& player, int choice) {
  if (event_id_ < 0 || choice < 0 ||
      static_cast<std::size_t>(choice) >= choices_.size()) {
    return;
  }

  if (event_id_ == 0) {
    if (choice == 0) {
      player.Heal(20);
    } else {
      player.GainGold(20);
    }
  } else if (event_id_ == 1) {
    if (choice == 0) {
      player.IncreaseMaxHp(10);
    } else {
      player.AddBlock(10);
    }
  } else {
    if (choice == 0) {
      player.GainGold(30);
    } else if (player.SpendGold(10)) {
      player.IncreaseMaxEnergy(1);
    }
  }

  // Prevent applying a single event multiple times
  event_id_ = -1;
  choices_.clear();
}
