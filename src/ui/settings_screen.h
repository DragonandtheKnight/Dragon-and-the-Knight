#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"
#include "game/difficulty.h"

ftxui::Component MakeSettingsScreen(Difficulty& difficulty,
                                    std::function<void()> on_back);
