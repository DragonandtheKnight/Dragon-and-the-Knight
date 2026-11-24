#pragma once

enum class GameScreen {
  kMainMenu,
  kSettings,
  kNewGame,
  kLoadGame,
  kMap,
  kNodeEvent,
  kBattle,
  kLoot,
  kExchangeCard,
  kShop,
  kInventory,
  kFloorTransition,
  kGameOver,
};

enum class GameEvent {
  kOpenSettings,
  kOpenNewGame,
  kOpenLoadGame,
  // Closes Settings, Inventory or Game Over, or cancels New Game, Load Game or
  // Exchange Card.
  kBack,
  kConfirmNewGame,
  kRestoreCheckpoint,
  // Normal and elite battles.
  kEnterBattle,
  kEnterBossBattle,
  kEnterShop,
  // Event and rest levels.
  kEnterNodeEvent,
  kSettleNodeEvent,
  kOpenBackpack,
  kWinBattle,
  kLoseBattle,
  kPickCard,
  kSkipCard,
  kFinishLoot,
  kBuyCard,
  kLeaveShop,
  kConfirmExchange,
  kNextFloor,
  kClearFinalFloor,
};

class GameStateMachine {
 public:
  GameScreen GetScreen() const;

  // Moves to the next screen and returns true if the board allows event on
  // the current screen. Otherwise returns false and stays put, because a
  // repeated key press before the screen switches is normal, not an error.
  bool Handle(GameEvent event);

 private:
  bool MoveTo(GameScreen screen);

  GameScreen screen_ = GameScreen::kMainMenu;
  // Inventory and Exchange Card return to the screen that opened them.
  GameScreen inventory_source_ = GameScreen::kMap;
  GameScreen exchange_source_ = GameScreen::kLoot;
  // Loot after a boss leads to the floor transition instead of the map.
  bool boss_battle_ = false;
};
