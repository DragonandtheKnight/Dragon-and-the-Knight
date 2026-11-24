#include "game/game_state_machine.h"

#include <set>
#include <vector>

#include "gtest/gtest.h"

constexpr GameEvent kAllEvents[] = {
    GameEvent::kOpenSettings,    GameEvent::kOpenNewGame,
    GameEvent::kOpenLoadGame,    GameEvent::kBack,
    GameEvent::kConfirmNewGame,  GameEvent::kRestoreCheckpoint,
    GameEvent::kEnterBattle,     GameEvent::kEnterBossBattle,
    GameEvent::kEnterShop,       GameEvent::kEnterNodeEvent,
    GameEvent::kSettleNodeEvent, GameEvent::kOpenBackpack,
    GameEvent::kWinBattle,       GameEvent::kLoseBattle,
    GameEvent::kPickCard,        GameEvent::kSkipCard,
    GameEvent::kFinishLoot,      GameEvent::kBuyCard,
    GameEvent::kLeaveShop,       GameEvent::kConfirmExchange,
    GameEvent::kNextFloor,       GameEvent::kClearFinalFloor,
};

// Drives a fresh machine through events that the board allows, failing the
// test if any of them is rejected.
static GameStateMachine After(const std::vector<GameEvent>& events) {
  GameStateMachine machine;
  for (GameEvent event : events) {
    EXPECT_TRUE(machine.Handle(event));
  }
  return machine;
}

static std::vector<GameEvent> ToMap() {
  return {GameEvent::kOpenNewGame, GameEvent::kConfirmNewGame};
}

static std::vector<GameEvent> FromMap(const std::vector<GameEvent>& events) {
  std::vector<GameEvent> path = ToMap();
  path.insert(path.end(), events.begin(), events.end());
  return path;
}

struct Transition {
  std::vector<GameEvent> path;
  GameEvent event;
  GameScreen expected;
};

// One row for each arrow on the State Machine board, plus Inventory from Loot
// and Battle, which the screen headers allow.
static std::vector<Transition> Transitions() {
  return {
      {{}, GameEvent::kOpenSettings, GameScreen::kSettings},
      {{GameEvent::kOpenSettings}, GameEvent::kBack, GameScreen::kMainMenu},
      {{}, GameEvent::kOpenNewGame, GameScreen::kNewGame},
      {{GameEvent::kOpenNewGame}, GameEvent::kBack, GameScreen::kMainMenu},
      {{GameEvent::kOpenNewGame}, GameEvent::kConfirmNewGame, GameScreen::kMap},
      {{}, GameEvent::kOpenLoadGame, GameScreen::kLoadGame},
      {{GameEvent::kOpenLoadGame}, GameEvent::kBack, GameScreen::kMainMenu},
      {{GameEvent::kOpenLoadGame},
       GameEvent::kRestoreCheckpoint,
       GameScreen::kMap},
      {ToMap(), GameEvent::kEnterBattle, GameScreen::kBattle},
      {ToMap(), GameEvent::kEnterBossBattle, GameScreen::kBattle},
      {ToMap(), GameEvent::kEnterShop, GameScreen::kShop},
      {ToMap(), GameEvent::kEnterNodeEvent, GameScreen::kNodeEvent},
      {ToMap(), GameEvent::kOpenBackpack, GameScreen::kInventory},
      {FromMap({GameEvent::kEnterNodeEvent}), GameEvent::kSettleNodeEvent,
       GameScreen::kMap},
      {FromMap({GameEvent::kEnterBattle}), GameEvent::kOpenBackpack,
       GameScreen::kInventory},
      {FromMap({GameEvent::kEnterBattle}), GameEvent::kWinBattle,
       GameScreen::kLoot},
      {FromMap({GameEvent::kEnterBattle}), GameEvent::kLoseBattle,
       GameScreen::kGameOver},
      {FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
       GameEvent::kPickCard, GameScreen::kExchangeCard},
      {FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
       GameEvent::kSkipCard, GameScreen::kLoot},
      {FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
       GameEvent::kOpenBackpack, GameScreen::kInventory},
      {FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
       GameEvent::kFinishLoot, GameScreen::kMap},
      {FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle}),
       GameEvent::kFinishLoot, GameScreen::kFloorTransition},
      {FromMap({GameEvent::kEnterShop}), GameEvent::kBuyCard,
       GameScreen::kExchangeCard},
      {FromMap({GameEvent::kEnterShop}), GameEvent::kOpenBackpack,
       GameScreen::kInventory},
      {FromMap({GameEvent::kEnterShop}), GameEvent::kLeaveShop,
       GameScreen::kMap},
      {FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle,
                GameEvent::kFinishLoot}),
       GameEvent::kNextFloor, GameScreen::kMap},
      {FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle,
                GameEvent::kFinishLoot}),
       GameEvent::kClearFinalFloor, GameScreen::kGameOver},
      {FromMap({GameEvent::kEnterBattle, GameEvent::kLoseBattle}),
       GameEvent::kBack, GameScreen::kMainMenu},
  };
}

TEST(GameStateMachineTest, StartsAtMainMenu) {
  EXPECT_EQ(GameStateMachine().GetScreen(), GameScreen::kMainMenu);
}

TEST(GameStateMachineTest, FollowsEveryTransitionOnTheBoard) {
  for (const Transition& transition : Transitions()) {
    GameStateMachine machine = After(transition.path);
    EXPECT_TRUE(machine.Handle(transition.event));
    EXPECT_EQ(machine.GetScreen(), transition.expected);
  }
}

// Any event without a row in Transitions() for the screen it starts on must
// be rejected and leave the screen unchanged.
TEST(GameStateMachineTest, RejectsEveryEventTheBoardDoesNotAllow) {
  const std::vector<std::vector<GameEvent>> paths_to_each_screen = {
      {},
      {GameEvent::kOpenSettings},
      {GameEvent::kOpenNewGame},
      {GameEvent::kOpenLoadGame},
      ToMap(),
      FromMap({GameEvent::kEnterNodeEvent}),
      FromMap({GameEvent::kEnterBattle}),
      FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
      FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle,
               GameEvent::kPickCard}),
      FromMap({GameEvent::kEnterShop}),
      FromMap({GameEvent::kOpenBackpack}),
      FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle,
               GameEvent::kFinishLoot}),
      FromMap({GameEvent::kEnterBattle, GameEvent::kLoseBattle}),
  };
  std::set<GameScreen> screens_checked;
  for (const std::vector<GameEvent>& path : paths_to_each_screen) {
    const GameScreen screen = After(path).GetScreen();
    screens_checked.insert(screen);
    std::set<GameEvent> allowed;
    for (const Transition& transition : Transitions()) {
      if (After(transition.path).GetScreen() == screen) {
        allowed.insert(transition.event);
      }
    }
    if (screen == GameScreen::kExchangeCard) {
      allowed.insert({GameEvent::kConfirmExchange, GameEvent::kBack});
    }
    if (screen == GameScreen::kInventory) {
      allowed.insert(GameEvent::kBack);
    }
    for (GameEvent event : kAllEvents) {
      if (allowed.count(event) > 0) {
        continue;
      }
      GameStateMachine machine = After(path);
      EXPECT_FALSE(machine.Handle(event))
          << "screen " << static_cast<int>(screen) << ", event "
          << static_cast<int>(event);
      EXPECT_EQ(machine.GetScreen(), screen);
    }
  }
  EXPECT_EQ(screens_checked.size(), 13u);
}

TEST(GameStateMachineTest, BackpackReturnsToTheScreenThatOpenedIt) {
  const std::vector<std::vector<GameEvent>> openers = {
      ToMap(),
      FromMap({GameEvent::kEnterShop}),
      FromMap({GameEvent::kEnterBattle}),
      FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle}),
  };
  for (const std::vector<GameEvent>& opener : openers) {
    GameStateMachine machine = After(opener);
    const GameScreen source = machine.GetScreen();
    ASSERT_TRUE(machine.Handle(GameEvent::kOpenBackpack));
    ASSERT_TRUE(machine.Handle(GameEvent::kBack));
    EXPECT_EQ(machine.GetScreen(), source);
  }
}

TEST(GameStateMachineTest, ExchangeCardReturnsToLootOnConfirmOrCancel) {
  for (GameEvent leave : {GameEvent::kConfirmExchange, GameEvent::kBack}) {
    GameStateMachine machine =
        After(FromMap({GameEvent::kEnterBattle, GameEvent::kWinBattle,
                       GameEvent::kPickCard}));
    ASSERT_TRUE(machine.Handle(leave));
    EXPECT_EQ(machine.GetScreen(), GameScreen::kLoot);
  }
}

TEST(GameStateMachineTest, ExchangeCardReturnsToShopOnConfirmOrCancel) {
  for (GameEvent leave : {GameEvent::kConfirmExchange, GameEvent::kBack}) {
    GameStateMachine machine =
        After(FromMap({GameEvent::kEnterShop, GameEvent::kBuyCard}));
    ASSERT_TRUE(machine.Handle(leave));
    EXPECT_EQ(machine.GetScreen(), GameScreen::kShop);
  }
}

TEST(GameStateMachineTest, BossLootStillLeadsToFloorTransitionAfterExchange) {
  GameStateMachine machine =
      After(FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle,
                     GameEvent::kPickCard, GameEvent::kConfirmExchange,
                     GameEvent::kOpenBackpack, GameEvent::kBack}));
  ASSERT_TRUE(machine.Handle(GameEvent::kFinishLoot));
  EXPECT_EQ(machine.GetScreen(), GameScreen::kFloorTransition);
}

TEST(GameStateMachineTest, NormalBattleAfterABossLeadsBackToMap) {
  GameStateMachine machine =
      After(FromMap({GameEvent::kEnterBossBattle, GameEvent::kWinBattle,
                     GameEvent::kFinishLoot, GameEvent::kNextFloor,
                     GameEvent::kEnterBattle, GameEvent::kWinBattle}));
  ASSERT_TRUE(machine.Handle(GameEvent::kFinishLoot));
  EXPECT_EQ(machine.GetScreen(), GameScreen::kMap);
}

TEST(GameStateMachineTest, PressingWinTwiceOpensLootOnce) {
  GameStateMachine machine = After(FromMap({GameEvent::kEnterBattle}));
  ASSERT_TRUE(machine.Handle(GameEvent::kWinBattle));
  EXPECT_FALSE(machine.Handle(GameEvent::kWinBattle));
  EXPECT_EQ(machine.GetScreen(), GameScreen::kLoot);
}

TEST(GameStateMachineTest, NewRunAfterGameOverStartsClean) {
  GameStateMachine machine = After(FromMap(
      {GameEvent::kEnterBossBattle, GameEvent::kLoseBattle, GameEvent::kBack,
       GameEvent::kOpenNewGame, GameEvent::kConfirmNewGame,
       GameEvent::kEnterBattle, GameEvent::kWinBattle}));
  ASSERT_TRUE(machine.Handle(GameEvent::kFinishLoot));
  EXPECT_EQ(machine.GetScreen(), GameScreen::kMap);
}
