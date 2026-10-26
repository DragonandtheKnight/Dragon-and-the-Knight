#include "ui/main_menu_screen.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/dom/node.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"

constexpr int kScreenWidth = 80;
constexpr int kScreenHeight = 24;

struct CellPosition {
  int x;
  int y;
};

// Searches cell by cell because Screen::ToString() mixes style escape codes
// into the text. Supports ASCII text only, where each character fills one cell.
static std::optional<CellPosition> FindText(const ftxui::Screen& screen,
                                            const std::string& text) {
  const int length = static_cast<int>(text.size());
  for (int y = 0; y < screen.dimy(); ++y) {
    for (int x = 0; x + length <= screen.dimx(); ++x) {
      bool matches = true;
      for (int i = 0; i < length && matches; ++i) {
        matches = screen.CellAt(x + i, y).character == std::string(1, text[i]);
      }
      if (matches) {
        return CellPosition{x, y};
      }
    }
  }
  return std::nullopt;
}

struct MenuEntry {
  const char* label;
  int arrow_downs_to_reach;
  const char* action;
  const char* test_name;
};

// Labels and order follow the main menu on the UI Design board. "Setting" also
// matches "Settings".
constexpr MenuEntry kEntries[] = {
    {"New Game", 0, "new game", "NewGame"},
    {"Load Game", 1, "load game", "LoadGame"},
    {"Setting", 2, "settings", "Settings"},
    {"Exit Game", 3, "exit", "ExitGame"},
};

class MainMenuScreenTest : public testing::Test {
 protected:
  MainMenuScreenTest() {
    MainMenuActions actions;
    actions.on_new_game = [this] { chosen_.push_back("new game"); };
    actions.on_load_game = [this] { chosen_.push_back("load game"); };
    actions.on_settings = [this] { chosen_.push_back("settings"); };
    actions.on_exit = [this] { chosen_.push_back("exit"); };
    menu_ = MakeMainMenuScreen(std::move(actions));
  }

  // The app draws a frame before handling each input, and mouse handling
  // depends on where the last frame placed each entry.
  ftxui::Screen Draw() {
    ftxui::Screen screen(kScreenWidth, kScreenHeight);
    ftxui::Render(screen, menu_->Render());
    return screen;
  }

  void Press(ftxui::Event event) {
    Draw();
    menu_->OnEvent(std::move(event));
  }

  void Click(const std::string& label) {
    std::optional<CellPosition> position = FindText(Draw(), label);
    ASSERT_TRUE(position.has_value()) << "\"" << label << "\" is not drawn";

    ftxui::Mouse mouse;
    mouse.button = ftxui::Mouse::Left;
    mouse.x = position->x;
    mouse.y = position->y;
    mouse.motion = ftxui::Mouse::Pressed;
    menu_->OnEvent(ftxui::Event::Mouse("", mouse));
    mouse.motion = ftxui::Mouse::Released;
    menu_->OnEvent(ftxui::Event::Mouse("", mouse));
  }

  std::vector<std::string> chosen_;
  ftxui::Component menu_;
};

TEST_F(MainMenuScreenTest, ShowsGameTitle) {
  EXPECT_TRUE(FindText(Draw(), "Dragon And The Knight").has_value());
}

TEST_F(MainMenuScreenTest, ListsEntriesTopToBottomInBoardOrder) {
  ftxui::Screen screen = Draw();
  int previous_row = -1;
  for (const MenuEntry& entry : kEntries) {
    std::optional<CellPosition> position = FindText(screen, entry.label);
    ASSERT_TRUE(position.has_value()) << "\"" << entry.label << "\" is missing";
    EXPECT_GT(position->y, previous_row) << "\"" << entry.label << "\"";
    previous_row = position->y;
  }
}

TEST_F(MainMenuScreenTest, MovingTheSelectionChoosesNothing) {
  Press(ftxui::Event::ArrowDown);
  Press(ftxui::Event::ArrowDown);
  Press(ftxui::Event::ArrowUp);

  EXPECT_TRUE(chosen_.empty());
}

class MainMenuEntryTest : public MainMenuScreenTest,
                          public testing::WithParamInterface<MenuEntry> {};

// Reaching "New Game" takes no arrow presses, so it is selected on open.
TEST_P(MainMenuEntryTest, EnterChoosesSelectedEntry) {
  for (int i = 0; i < GetParam().arrow_downs_to_reach; ++i) {
    Press(ftxui::Event::ArrowDown);
  }
  Press(ftxui::Event::Return);

  EXPECT_EQ(chosen_, std::vector<std::string>{GetParam().action});
}

TEST_P(MainMenuEntryTest, ClickChoosesEntry) {
  ASSERT_NO_FATAL_FAILURE(Click(GetParam().label));

  EXPECT_EQ(chosen_, std::vector<std::string>{GetParam().action});
}

INSTANTIATE_TEST_SUITE_P(BoardEntries, MainMenuEntryTest,
                         testing::ValuesIn(kEntries),
                         [](const testing::TestParamInfo<MenuEntry>& info) {
                           return std::string(info.param.test_name);
                         });

// The UI preview builds the menu without actions, so choosing an entry there
// must not throw std::bad_function_call.
TEST(MainMenuScreenWithoutActionsTest, ChoosingAnEntryDoesNotThrow) {
  ftxui::Component menu = MakeMainMenuScreen({});
  ftxui::Screen screen(kScreenWidth, kScreenHeight);
  ftxui::Render(screen, menu->Render());

  EXPECT_NO_THROW(menu->OnEvent(ftxui::Event::Return));
}
