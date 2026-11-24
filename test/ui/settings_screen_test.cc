#include "ui/settings_screen.h"

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "game/difficulty.h"
#include "gtest/gtest.h"
#include "ui/screen_text.h"

class SettingsScreenTest : public testing::Test {
 protected:
  SettingsScreenTest()
      : screen_(MakeSettingsScreen(difficulty_, [this] { ++backs_; })) {}

  Difficulty difficulty_ = Difficulty::kSimple;
  int backs_ = 0;
  ftxui::Component screen_;
};

TEST_F(SettingsScreenTest, ShowsEveryLevel) {
  EXPECT_TRUE(Shows(screen_, "Game Level"));
  EXPECT_TRUE(Shows(screen_, "Simple"));
  EXPECT_TRUE(Shows(screen_, "Normal"));
  EXPECT_TRUE(Shows(screen_, "Hard"));
}

TEST_F(SettingsScreenTest, RightArrowRaisesTheLevel) {
  Press(screen_, ftxui::Event::ArrowRight);
  EXPECT_EQ(difficulty_, Difficulty::kNormal);

  Press(screen_, ftxui::Event::ArrowRight);
  EXPECT_EQ(difficulty_, Difficulty::kHard);
}

TEST_F(SettingsScreenTest, LeftArrowLowersTheLevel) {
  Press(screen_, ftxui::Event::ArrowRight);
  Press(screen_, ftxui::Event::ArrowLeft);
  EXPECT_EQ(difficulty_, Difficulty::kSimple);
}

TEST_F(SettingsScreenTest, ClickingALevelChoosesIt) {
  ASSERT_TRUE(Click(screen_, "Hard"));
  EXPECT_EQ(difficulty_, Difficulty::kHard);
}

TEST_F(SettingsScreenTest, OpensOnTheCurrentLevel) {
  Difficulty difficulty = Difficulty::kHard;
  ftxui::Component screen = MakeSettingsScreen(difficulty, {});

  Press(screen, ftxui::Event::ArrowLeft);
  EXPECT_EQ(difficulty, Difficulty::kNormal);
}

TEST_F(SettingsScreenTest, EscapeGoesBack) {
  Press(screen_, ftxui::Event::Escape);
  EXPECT_EQ(backs_, 1);
}

TEST_F(SettingsScreenTest, BackButtonGoesBack) {
  ASSERT_TRUE(Click(screen_, "Back"));
  EXPECT_EQ(backs_, 1);
}

TEST_F(SettingsScreenTest, BackWithoutCallbackDoesNotThrow) {
  Difficulty difficulty = Difficulty::kSimple;
  ftxui::Component screen = MakeSettingsScreen(difficulty, {});
  Draw(screen);
  EXPECT_NO_THROW(screen->OnEvent(ftxui::Event::Escape));
}

TEST_F(SettingsScreenTest, ShowsTheHardModeWarning) {
  EXPECT_TRUE(Shows(screen_, "Hard: if you die"));
}

TEST_F(SettingsScreenTest, RightArrowStopsAtHard) {
  for (int i = 0; i < 5; ++i) {
    Press(screen_, ftxui::Event::ArrowRight);
  }
  EXPECT_EQ(difficulty_, Difficulty::kHard);
}

TEST_F(SettingsScreenTest, LeftArrowStopsAtSimple) {
  Press(screen_, ftxui::Event::ArrowLeft);
  EXPECT_EQ(difficulty_, Difficulty::kSimple);
}

TEST_F(SettingsScreenTest, EnterOnBackButtonGoesBackWithoutChangingLevel) {
  Press(screen_, ftxui::Event::ArrowRight);
  Press(screen_, ftxui::Event::ArrowDown);
  Press(screen_, ftxui::Event::Return);

  EXPECT_EQ(backs_, 1);
  EXPECT_EQ(difficulty_, Difficulty::kNormal);
}

TEST_F(SettingsScreenTest, LeftRightOnBackButtonDoesNotChangeLevel) {
  Press(screen_, ftxui::Event::ArrowDown);
  Press(screen_, ftxui::Event::ArrowRight);

  EXPECT_EQ(difficulty_, Difficulty::kSimple);
  EXPECT_EQ(backs_, 0);
}
