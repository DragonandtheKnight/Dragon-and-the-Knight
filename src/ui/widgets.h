#pragma once

#include <string>
#include <vector>

#include "ftxui/dom/elements.hpp"

class Card;
class Player;

struct KeyHint {
  std::string keys;
  std::string action;
};

ftxui::Element RenderStatusBar(const Player& player,
                               const std::string& location,
                               const std::string& progress);

ftxui::Element RenderCard(const Card& card, int number, bool selected);

ftxui::Element RenderKeyHints(const std::vector<KeyHint>& hints);
