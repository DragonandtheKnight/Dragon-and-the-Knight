#include "ui/settings_screen.h"

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"

// In the same order as the labels in SettingsState::labels.
constexpr std::array<Difficulty, 3> kLevels = {
    Difficulty::kSimple, Difficulty::kNormal, Difficulty::kHard};

// Toggle keeps pointers to its labels and selection, so they live as long as
// the screen does.
struct SettingsState {
  std::vector<std::string> labels = {"Simple", "Normal", "Hard"};
  int selected = 0;
};

ftxui::Component MakeSettingsScreen(Difficulty& difficulty,
                                    std::function<void()> on_back) {
  auto state = std::make_shared<SettingsState>();
  for (int i = 0; i < static_cast<int>(kLevels.size()); ++i) {
    if (kLevels[i] == difficulty) {
      state->selected = i;
    }
  }

  ftxui::MenuOption level_option = ftxui::MenuOption::Horizontal();
  level_option.on_change = [state, &difficulty] {
    difficulty = kLevels[state->selected];
  };
  ftxui::Component level =
      ftxui::Menu(&state->labels, &state->selected, level_option);

  auto back = [on_back = std::move(on_back)] {
    if (on_back) {
      on_back();
    }
  };
  ftxui::Component back_button =
      ftxui::Button("Back", back, ftxui::ButtonOption::Border());

  ftxui::Component layout = ftxui::Container::Vertical({level, back_button});
  ftxui::Component screen = ftxui::Renderer(layout, [=] {
    return ftxui::vbox({
               ftxui::text("Settings") | ftxui::bold | ftxui::center,
               ftxui::separator(),
               ftxui::hbox({ftxui::text("Game Level : "), level->Render()}),
               ftxui::text("Hard: if you die, the run starts again from the "
                           "beginning.") |
                   ftxui::dim,
               ftxui::separator(),
               back_button->Render() | ftxui::center,
               ftxui::text("<- -> change level   Esc back") | ftxui::dim,
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
