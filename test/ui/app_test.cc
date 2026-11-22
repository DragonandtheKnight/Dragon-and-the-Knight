#include "ui/app.h"

#include <cstdint>
#include <string>

#include "ftxui/component/component_base.hpp"
#include "ftxui/component/event.hpp"
#include "game/difficulty.h"
#include "game/game.h"
#include "game/game_state_machine.h"
#include "gtest/gtest.h"
#include "ui/screen_text.h"

namespace {

constexpr uint32_t kSeed = 99;

}  // namespace

class AppTest : public testing::Test {
 protected:
  AppTest() : app_([this] { ++exits_; }, kSeed), root_(app_.Root()) {}

  void OpenSettings() { ASSERT_TRUE(Click(root_, "Settings")); }

  void StartRun() {
    ASSERT_TRUE(Click(root_, "New Game"));
    ASSERT_TRUE(Click(root_, "Start"));
    ASSERT_EQ(app_.GetScreen(), GameScreen::kMap);
  }

  // One key press that moves the run on, the way a player would.
  void Step() {
    switch (app_.GetScreen()) {
      case GameScreen::kMap:
        Press(root_, ftxui::Event::Character('1'));
        return;
      case GameScreen::kBattle:
        Press(root_, app_.GetGame()->GetCombat()->GetCurrentAp() == 0
                         ? ftxui::Event::Character('e')
                         : ftxui::Event::Return);
        return;
      case GameScreen::kLoot:
        Press(root_, ftxui::Event::Character('s'));
        return;
      case GameScreen::kShop:
        Press(root_, ftxui::Event::Escape);
        return;
      default:
        Press(root_, ftxui::Event::Return);
        return;
    }
  }

  // Steps until the screen is target, at most max_steps times.
  bool StepUntil(GameScreen target, int max_steps) {
    for (int i = 0; i < max_steps && app_.GetScreen() != target; ++i) {
      Step();
    }
    return app_.GetScreen() == target;
  }

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
  App app({}, kSeed);
  ftxui::Component root = app.Root();

  EXPECT_NO_THROW(Click(root, "Exit Game"));
  EXPECT_EQ(app.GetScreen(), GameScreen::kMainMenu);
}

TEST_F(AppTest, NewGameShowsTheDifficultyAndBackReturns) {
  ASSERT_TRUE(Click(root_, "New Game"));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kNewGame);
  EXPECT_TRUE(Shows(root_, "Difficulty: Simple"));

  Press(root_, ftxui::Event::Escape);
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_EQ(app_.GetGame(), nullptr);
}

TEST_F(AppTest, LoadGameSaysThereIsNoSaveAndBackReturns) {
  ASSERT_TRUE(Click(root_, "Load Game"));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kLoadGame);
  EXPECT_TRUE(Shows(root_, "No saved run."));

  ASSERT_TRUE(Click(root_, "Back"));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
}

TEST_F(AppTest, StartBeginsARunOnTheChosenDifficulty) {
  OpenSettings();
  ASSERT_TRUE(Click(root_, "Hard"));
  Press(root_, ftxui::Event::Escape);

  StartRun();

  ASSERT_NE(app_.GetGame(), nullptr);
  EXPECT_EQ(app_.GetGame()->GetDifficulty(), Difficulty::kHard);
  EXPECT_TRUE(Shows(root_, "Floor 1"));
  EXPECT_TRUE(Shows(root_, "Dragon"));
}

TEST_F(AppTest, BackpackOpensFromTheMapAndReturnsToIt) {
  StartRun();

  Press(root_, ftxui::Event::Character('b'));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kInventory);
  EXPECT_TRUE(Shows(root_, "Deck (8 cards"));

  Press(root_, ftxui::Event::Escape);
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMap);
}

TEST_F(AppTest, FirstPathStartsABattle) {
  StartRun();

  Press(root_, ftxui::Event::Character('1'));

  EXPECT_EQ(app_.GetScreen(), GameScreen::kBattle);
  EXPECT_TRUE(Shows(root_, "End Turn"));
  EXPECT_TRUE(Shows(root_, "3/10"));
}

TEST_F(AppTest, DeckOpensFromBattleAndReturnsToIt) {
  StartRun();
  Press(root_, ftxui::Event::Character('1'));

  Press(root_, ftxui::Event::Character('b'));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kInventory);
  Press(root_, ftxui::Event::Escape);
  EXPECT_EQ(app_.GetScreen(), GameScreen::kBattle);
}

TEST_F(AppTest, EndTurnRefillsTheAp) {
  StartRun();
  Press(root_, ftxui::Event::Character('1'));
  Press(root_, ftxui::Event::Return);
  Press(root_, ftxui::Event::Return);
  ASSERT_LT(app_.GetGame()->GetCombat()->GetCurrentAp(), 3);

  Press(root_, ftxui::Event::Character('e'));

  EXPECT_EQ(app_.GetGame()->GetCombat()->GetCurrentAp(), 3);
}

TEST_F(AppTest, WinningShowsLootAndSkippingReturnsToTheMap) {
  StartRun();
  Press(root_, ftxui::Event::Character('1'));

  ASSERT_TRUE(StepUntil(GameScreen::kLoot, 300));
  EXPECT_TRUE(Shows(root_, "Victory!"));

  Press(root_, ftxui::Event::Character('s'));
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMap);
  EXPECT_EQ(app_.GetGame()->GetCombat(), nullptr);
}

TEST_F(AppTest, TakingALootCardExchangesItThenContinues) {
  StartRun();
  Press(root_, ftxui::Event::Character('1'));
  ASSERT_TRUE(StepUntil(GameScreen::kLoot, 300));
  const std::string loot = app_.GetGame()->GetLootCards()[0].GetName();

  Press(root_, ftxui::Event::Return);
  ASSERT_EQ(app_.GetScreen(), GameScreen::kExchangeCard);
  Press(root_, ftxui::Event::Return);

  ASSERT_EQ(app_.GetScreen(), GameScreen::kLoot);
  EXPECT_EQ(app_.GetGame()->GetPlayer().GetDeck().GetCards().back().GetName(),
            loot);
  EXPECT_TRUE(Shows(root_, "Continue"));
  Press(root_, ftxui::Event::Return);
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMap);
}

TEST_F(AppTest, CancellingTheExchangeReturnsToTheLoot) {
  StartRun();
  Press(root_, ftxui::Event::Character('1'));
  ASSERT_TRUE(StepUntil(GameScreen::kLoot, 300));

  Press(root_, ftxui::Event::Return);
  Press(root_, ftxui::Event::Escape);

  EXPECT_EQ(app_.GetScreen(), GameScreen::kLoot);
  EXPECT_EQ(static_cast<int>(app_.GetGame()->GetLootCards().size()),
            Game::kLootChoices);
}

// Plays three floors with nothing but key presses, through every kind of
// level the map happens to offer, and back to the main menu.
TEST_F(AppTest, WholeRunEndsInVictoryAndReturnsToTheMainMenu) {
  StartRun();

  ASSERT_TRUE(StepUntil(GameScreen::kGameOver, 20000));
  EXPECT_TRUE(Shows(root_, "You Win"));
  EXPECT_TRUE(Shows(root_, "Final Level: 3"));

  Press(root_, ftxui::Event::Return);
  EXPECT_EQ(app_.GetScreen(), GameScreen::kMainMenu);
  EXPECT_EQ(app_.GetGame(), nullptr);
}

TEST_F(AppTest, ANewRunAfterTheLastStartsFresh) {
  StartRun();
  ASSERT_TRUE(StepUntil(GameScreen::kGameOver, 20000));
  Press(root_, ftxui::Event::Return);

  StartRun();

  EXPECT_EQ(app_.GetGame()->GetFloor(), 1);
  EXPECT_EQ(app_.GetGame()->GetPlayer().GetGold(), 0);
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
