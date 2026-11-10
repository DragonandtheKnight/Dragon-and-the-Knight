#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

class FloorMap;
class Player;

struct MapScreenActions {
  std::function<void(int level_id)> on_enter_level;
  std::function<void()> on_open_backpack;
};

ftxui::Component MakeMapScreen(const Player& player, const FloorMap& map,
                               int floor, MapScreenActions actions);
