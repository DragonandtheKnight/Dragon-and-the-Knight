#include "ui/app.h"

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "game/difficulty.h"
#include "game/game_state_machine.h"
#include "gtest/gtest.h"
#include "ui/screen_text.h"

class AppTest : public testing::Test {
 protected:
  AppTest() : app_([this] { ++exits_; }), root_(app_.Root()) {}

  void OpenSettings() { ASSERT_TRUE(Click(root_, "Settings")); }

  int exits_ = 0;
  App app_;
  ftxui::Component root_;
};

TEST_F(AppTest, StartsOnTheMainMenu) {
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_TRUE(Shows(root_, "Dragon And The Knight"));
}

TEST_F(AppTest, SettingsButtonOpensSettings) {
  OpenSettings();

  EXPECT_EQ(app_.GetScreen(), GameScreen::kSettings);
  EXPECT_TRUE(Shows(root_, "Game Level"));
  EXPECT_FALSE(Shows(root_, "Dragon And The Knight"));
}

TEST_F(AppTest, EscapeInSettingsReturnsToTheMainMenu) {
  OpenSettings();
  Press(root_, ftxui::Event::Escape);

  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_TRUE(Shows(root_, "Dragon And The Knight"));
}

TEST_F(AppTest, BackButtonInSettingsReturnsToTheMainMenu) {
  OpenSettings();
  ASSERT_TRUE(Click(root_, "Back"));

  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_TRUE(Shows(root_, "Dragon And The Knight"));
}

TEST_F(AppTest, StartsOnSimpleDifficulty) {
  EXPECT_EQ(app_.GetDifficulty(), Difficulty::kSimple);
}

TEST_F(AppTest, ChosenDifficultyIsKeptAfterLeavingSettings) {
  OpenSettings();
  ASSERT_TRUE(Click(root_, "Hard"));
  Press(root_, ftxui::Event::Escape);

  EXPECT_EQ(app_.GetDifficulty(), Difficulty::kHard);
}

TEST_F(AppTest, SettingsReopensOnTheChosenDifficulty) {
  OpenSettings();
  ASSERT_TRUE(Click(root_, "Hard"));
  Press(root_, ftxui::Event::Escape);

  OpenSettings();
  Press(root_, ftxui::Event::ArrowLeft);

  EXPECT_EQ(app_.GetDifficulty(), Difficulty::kNormal);
}

TEST_F(AppTest, ExitGameCallsOnExitOnce) {
  ASSERT_TRUE(Click(root_, "Exit Game"));

  EXPECT_EQ(exits_, 1);
}

TEST_F(AppTest, ExitGameWithoutOnExitDoesNothing) {
  App app({});
  ftxui::Component root = app.Root();

  EXPECT_NO_THROW(Click(root, "Exit Game"));
  EXPECT_EQ(app.GetScreen(), GameScreen::kMainMenu);
}

// New Game and Load Game both lead to the Map screen, which is not built yet.
TEST_F(AppTest, NewGameAndLoadGameKeepTheMainMenu) {
  ASSERT_TRUE(Click(root_, "New Game"));
  ASSERT_TRUE(Click(root_, "Load Game"));

  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_TRUE(Shows(root_, "Dragon And The Knight"));
}

// A second key press can reach the old screen before the next redraw swaps it
// out. The state machine ignores it, so the player stays on the main menu.
TEST_F(AppTest, SecondEscapeBeforeRedrawIsIgnored) {
  OpenSettings();
  Draw(root_);

  root_->OnEvent(ftxui::Event::Escape);
  root_->OnEvent(ftxui::Event::Escape);

  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_TRUE(Shows(root_, "Dragon And The Knight"));
}
