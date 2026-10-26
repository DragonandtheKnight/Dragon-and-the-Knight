#pragma once

#include <functional>
#include <vector>

#include "ftxui/component/component_base.hpp"

class Card;

struct LootScreenActions {
  std::function<void(int choice_index)> on_take_card;
  std::function<void()> on_skip;
  std::function<void()> on_open_backpack;
};

ftxui::Component MakeLootScreen(int gold_reward,
                                const std::vector<Card>& card_choices,
                                LootScreenActions actions);
