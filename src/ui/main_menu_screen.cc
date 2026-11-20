#include "ui/main_menu_screen.h"

#include <functional>
#include <string>
#include <utility>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/dom/elements.hpp"

static ftxui::Component MenuButton(const std::string& label,
                                   std::function<void()> action) {
  return ftxui::Button(
      label,
      [action = std::move(action)] {
        if (action) {
          action();
        }
      },
      ftxui::ButtonOption::Border());
}

ftxui::Component MakeMainMenuScreen(MainMenuActions actions) {
  ftxui::Component buttons = ftxui::Container::Vertical({
      MenuButton("New Game", std::move(actions.on_new_game)),
      MenuButton("Load Game", std::move(actions.on_load_game)),
      MenuButton("Settings", std::move(actions.on_settings)),
      MenuButton("Exit Game", std::move(actions.on_exit)),
  });
  return ftxui::Renderer(buttons, [buttons] {
    return ftxui::vbox({
               ftxui::text("Dragon And The Knight") | ftxui::bold |
                   ftxui::center,
               ftxui::separator(),
               buttons->Render() | ftxui::center,
           }) |
           ftxui::border;
  });
}
