#include "game/game_state_machine.h"

GameScreen GameStateMachine::GetScreen() const { return screen_; }

bool GameStateMachine::MoveTo(GameScreen screen) {
  screen_ = screen;
  return true;
}

bool GameStateMachine::Handle(GameEvent event) {
  switch (screen_) {
    case GameScreen::kMainMenu:
      if (event == GameEvent::kOpenSettings) {
        return MoveTo(GameScreen::kSettings);
      }
      if (event == GameEvent::kOpenNewGame) {
        return MoveTo(GameScreen::kNewGame);
      }
      if (event == GameEvent::kOpenLoadGame) {
        return MoveTo(GameScreen::kLoadGame);
      }
      return false;

    case GameScreen::kSettings:
    case GameScreen::kGameOver:
      if (event == GameEvent::kBack) {
        return MoveTo(GameScreen::kMainMenu);
      }
      return false;

    case GameScreen::kNewGame:
    case GameScreen::kLoadGame: {
      if (event == GameEvent::kBack) {
        return MoveTo(GameScreen::kMainMenu);
      }
      const GameEvent start = screen_ == GameScreen::kNewGame
                                  ? GameEvent::kConfirmNewGame
                                  : GameEvent::kRestoreCheckpoint;
      if (event == start) {
        boss_battle_ = false;
        return MoveTo(GameScreen::kMap);
      }
      return false;
    }

    case GameScreen::kMap:
      if (event == GameEvent::kEnterBattle ||
          event == GameEvent::kEnterBossBattle) {
        boss_battle_ = event == GameEvent::kEnterBossBattle;
        return MoveTo(GameScreen::kBattle);
      }
      if (event == GameEvent::kEnterShop) {
        return MoveTo(GameScreen::kShop);
      }
      if (event == GameEvent::kEnterNodeEvent) {
        return MoveTo(GameScreen::kNodeEvent);
      }
      break;

    case GameScreen::kNodeEvent:
      if (event == GameEvent::kSettleNodeEvent) {
        return MoveTo(GameScreen::kMap);
      }
      return false;

    case GameScreen::kBattle:
      if (event == GameEvent::kWinBattle) {
        return MoveTo(GameScreen::kLoot);
      }
      if (event == GameEvent::kLoseBattle) {
        return MoveTo(GameScreen::kGameOver);
      }
      break;

    case GameScreen::kLoot:
      if (event == GameEvent::kPickCard) {
        exchange_source_ = GameScreen::kLoot;
        return MoveTo(GameScreen::kExchangeCard);
      }
      // Skipping one card keeps the rest of the loot open.
      if (event == GameEvent::kSkipCard) {
        return true;
      }
      if (event == GameEvent::kFinishLoot) {
        return MoveTo(boss_battle_ ? GameScreen::kFloorTransition
                                   : GameScreen::kMap);
      }
      break;

    case GameScreen::kExchangeCard:
      if (event == GameEvent::kConfirmExchange || event == GameEvent::kBack) {
        return MoveTo(exchange_source_);
      }
      return false;

    case GameScreen::kShop:
      if (event == GameEvent::kBuyCard) {
        exchange_source_ = GameScreen::kShop;
        return MoveTo(GameScreen::kExchangeCard);
      }
      if (event == GameEvent::kLeaveShop) {
        return MoveTo(GameScreen::kMap);
      }
      break;

    case GameScreen::kInventory:
      if (event == GameEvent::kBack) {
        return MoveTo(inventory_source_);
      }
      return false;

    case GameScreen::kFloorTransition:
      if (event == GameEvent::kNextFloor) {
        boss_battle_ = false;
        return MoveTo(GameScreen::kMap);
      }
      if (event == GameEvent::kClearFinalFloor) {
        return MoveTo(GameScreen::kGameOver);
      }
      return false;
  }

  // Only Map, Battle, Loot and Shop reach here: each can open the read-only
  // Inventory.
  if (event == GameEvent::kOpenBackpack) {
    inventory_source_ = screen_;
    return MoveTo(GameScreen::kInventory);
  }
  return false;
}
