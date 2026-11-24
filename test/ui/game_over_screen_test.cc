#include "ui/game_over_screen.h"

#include <optional>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/screen/color.hpp"
#include "ftxui/screen/screen.hpp"
#include "gtest/gtest.h"
#include "ui/screen_text.h"

TEST(GameOverScreenTest, VictoryShowsWinAndTheRunTotals) {
  ftxui::Component screen =
      MakeGameOverScreen(GameOutcome::kVictory, 42, 4, {});

  EXPECT_TRUE(Shows(screen, "You Win"));
  EXPECT_FALSE(Shows(screen, "You Lose"));
  EXPECT_TRUE(Shows(screen, "Total Gold Gain: 42"));
  EXPECT_TRUE(Shows(screen, "Final Level: 4"));
}

TEST(GameOverScreenTest, DefeatShowsLose) {
  ftxui::Component screen = MakeGameOverScreen(GameOutcome::kDefeat, 0, 1, {});

  EXPECT_TRUE(Shows(screen, "You Lose"));
  EXPECT_FALSE(Shows(screen, "You Win"));
}

TEST(GameOverScreenTest, EnterOnTheButtonGoesToMainMenu) {
  int backs = 0;
  ftxui::Component screen =
      MakeGameOverScreen(GameOutcome::kDefeat, 0, 1, [&backs] { ++backs; });

  Press(screen, ftxui::Event::Return);
  EXPECT_EQ(backs, 1);
}

TEST(GameOverScreenTest, ClickingTheButtonGoesToMainMenu) {
  int backs = 0;
  ftxui::Component screen =
      MakeGameOverScreen(GameOutcome::kVictory, 0, 1, [&backs] { ++backs; });

  ASSERT_TRUE(Click(screen, "Back to Main Menu"));
  EXPECT_EQ(backs, 1);
}

TEST(GameOverScreenTest, EscapeGoesToMainMenu) {
  int backs = 0;
  ftxui::Component screen =
      MakeGameOverScreen(GameOutcome::kVictory, 0, 1, [&backs] { ++backs; });

  Press(screen, ftxui::Event::Escape);
  EXPECT_EQ(backs, 1);
}

TEST(GameOverScreenTest, BackWithoutCallbackDoesNotThrow) {
  ftxui::Component screen = MakeGameOverScreen(GameOutcome::kVictory, 0, 1, {});
  Draw(screen);
  EXPECT_NO_THROW(screen->OnEvent(ftxui::Event::Return));
}

// The colour is the only cue besides the words, so both are part of the
// design: green for a win, red for a loss.
TEST(GameOverScreenTest, OutcomeIsColouredGreenForWinRedForLoss) {
  ftxui::Screen win = Draw(MakeGameOverScreen(GameOutcome::kVictory, 0, 1, {}));
  std::optional<CellPosition> win_at = FindText(win, "You Win");
  ASSERT_TRUE(win_at.has_value());
  EXPECT_EQ(win.CellAt(win_at->x, win_at->y).foreground_color,
            ftxui::Color(ftxui::Color::Green));

  ftxui::Screen loss = Draw(MakeGameOverScreen(GameOutcome::kDefeat, 0, 1, {}));
  std::optional<CellPosition> loss_at = FindText(loss, "You Lose");
  ASSERT_TRUE(loss_at.has_value());
  EXPECT_EQ(loss.CellAt(loss_at->x, loss_at->y).foreground_color,
            ftxui::Color(ftxui::Color::Red));
}

TEST(GameOverScreenTest, OtherKeysDoNothing) {
  int backs = 0;
  ftxui::Component screen =
      MakeGameOverScreen(GameOutcome::kVictory, 0, 1, [&backs] { ++backs; });

  Press(screen, ftxui::Event::Character('x'));
  Press(screen, ftxui::Event::ArrowDown);
  EXPECT_EQ(backs, 0);
}
