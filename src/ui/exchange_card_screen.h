#pragma once

#include <functional>

#include "ftxui/component/component_base.hpp"

class Card;
class Deck;

struct ExchangeCardScreenActions {
  std::function<void(int deck_index)> on_exchange;
  std::function<void()> on_cancel;
};

ftxui::Component MakeExchangeCardScreen(const Card& card_to_add,
                                        const Deck& deck,
                                        ExchangeCardScreenActions actions);
