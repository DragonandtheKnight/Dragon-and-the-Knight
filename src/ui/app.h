#pragma once

#include <functional>
#include <memory>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "game/difficulty.h"

class Card;
class Game;

class App {
 public:
  App();
  ~App();

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  void Run();

 private:
  void ShowMainMenu();
  void ShowSettings();
  void ShowMap();
  void ShowBattle();
  void ShowLoot();
  void ShowExchangeCard(const Card& card_to_add);
  void ShowShop();
  void ShowInventory(std::function<void()> on_back);
  void ShowGameOver();

  void Show(ftxui::Component screen);

  ftxui::ScreenInteractive screen_;
  ftxui::Component root_;
  std::unique_ptr<Game> game_;
  Difficulty difficulty_ = Difficulty::kSimple;
};
