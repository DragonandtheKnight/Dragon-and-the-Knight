#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

class Battle;
class Player;

struct BattleScreenActions {
  std::function<void(int hand_index, int target_index)> on_play_card;
  std::function<void()> on_end_turn;
  std::function<void()> on_open_deck;
};

ftxui::Component MakeBattleScreen(const Player& player, const Battle& battle,
                                  int floor, BattleScreenActions actions);
