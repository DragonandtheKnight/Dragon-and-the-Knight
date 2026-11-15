#include "level/rest.h"

#include <algorithm>

#include "level/level_type.h"
#include "player/player.h"

Rest::Rest(int id, int heal_amount)
    : Level(id, LevelType::kRest, false),
      heal_amount_(std::max(0, heal_amount)) {}

void Rest::Enter(Player& player) { HealPlayer(player); }

void Rest::HealPlayer(Player& player) { player.Heal(heal_amount_); }
