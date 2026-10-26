#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

struct MainMenuActions {
  std::function<void()> on_new_game;
  std::function<void()> on_load_game;
  std::function<void()> on_settings;
  std::function<void()> on_exit;
};

ftxui::Component MakeMainMenuScreen(MainMenuActions actions);
