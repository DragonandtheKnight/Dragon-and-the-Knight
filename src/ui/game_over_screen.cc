#include "ui/game_over_screen.h"

#include <functional>
#include <string>
#include <utility>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

ftxui::Component MakeGameOverScreen(
    GameOutcome outcome, int total_gold_gained, int final_level,
    std::function<void()> on_back_to_main_menu) {
  auto back = [on_back = std::move(on_back_to_main_menu)] {
    if (on_back) {
      on_back();
    }
  };
  ftxui::Component back_button =
      ftxui::Button("Back to Main Menu", back, ftxui::ButtonOption::Border());

  const bool won = outcome == GameOutcome::kVictory;
  ftxui::Component screen = ftxui::Renderer(back_button, [=] {
    return ftxui::vbox({
               ftxui::text(won ? "You Win" : "You Lose") | ftxui::bold |
                   ftxui::color(won ? ftxui::Color::Green : ftxui::Color::Red) |
                   ftxui::center,
               ftxui::separator(),
               ftxui::text("Total Gold Gain: " +
                           std::to_string(total_gold_gained)),
               ftxui::text("Final Level: " + std::to_string(final_level)),
               ftxui::separator(),
               back_button->Render() | ftxui::center,
           }) |
           ftxui::border;
  });
  return ftxui::CatchEvent(screen, [back](const ftxui::Event& event) {
    if (event == ftxui::Event::Escape) {
      back();
      return true;
    }
    return false;
  });
}
