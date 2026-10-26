#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

class Player;
class Shop;

struct ShopScreenActions {
  std::function<void(int card_index)> on_buy_card;
  std::function<void()> on_buy_heal;
  std::function<void()> on_leave;
  std::function<void()> on_open_backpack;
};

ftxui::Component MakeShopScreen(const Player& player, const Shop& shop,
                                int floor, ShopScreenActions actions);
