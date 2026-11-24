#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

class Player;

ftxui::Component MakeInventoryScreen(const Player& player,
                                     std::function<void()> on_back);
