#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

enum class GameOutcome {
  kVictory,
  kDefeat,
};

ftxui::Component MakeGameOverScreen(GameOutcome outcome, int total_gold_gained,
                                    int final_level,
                                    std::function<void()> on_back_to_main_menu);
