#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "card/card.h"
#include "combat/combat.h"
#include "enemy/enemy.h"
#include "game/difficulty.h"
#include "game/rng.h"
#include "level/level.h"
#include "level/shop.h"
#include "map/floor_map.h"
#include "player/player.h"

enum class BattleResult {
  kNone,
  kOngoing,
  kWon,
  kLost,
};

class Game {
 public:
  static constexpr int kFloors = 3;
  static constexpr int kLootChoices = 3;
  static constexpr int kHealPrice = 20;
  static constexpr int kHealAmount = 15;

  Game(Difficulty difficulty, uint32_t seed);
  // Levels on the map keep a reference to rng_, so a Game never moves.
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  const Player& GetPlayer() const;
  const FloorMap& GetMap() const;
  // From 1 to kFloors.
  int GetFloor() const;
  bool IsFinalFloor() const;
  Difficulty GetDifficulty() const;

  bool EnterLevel(int id);
  const Level* GetCurrentLevel() const;

  // Battle. GetCombat is null outside a battle and its loot.
  const Combat* GetCombat() const;
  bool IsBossBattle() const;
  // Return false and change nothing unless a battle is ongoing and the move
  // is legal.
  bool PlayCard(int hand_index, int target_slot);
  bool EndTurn();
  BattleResult GetBattleResult() const;

  // Loot, filled when a battle is won.
  int GetLootGold() const;
  const std::vector<Card>& GetLootCards() const;
  // Puts loot card index up for exchange.
  bool PickLootCard(int index);
  // Ends the battle and drops any loot card not taken.
  void FinishLoot();

  // Shop. GetShop is null unless the current level is a shop.
  const Shop* GetShop() const;
  // Puts shop card index up for exchange if the knight can afford it. Gold is
  // spent only when the exchange is confirmed.
  bool PickShopCard(int index);
  // Spends kHealPrice gold to heal kHealAmount HP.
  bool BuyHeal();

  // The card picked from loot or a shop, waiting to replace a deck card.
  const Card* GetPendingCard() const;
  // Replaces deck card deck_index with the pending card.
  bool ConfirmExchange(int deck_index);
  void CancelExchange();

  // Applies a choice of the current event level.
  bool ChooseEventOption(int choice);

  // Builds the next floor's map. Does nothing on the final floor.
  bool NextFloor();

 private:
  struct PendingCard {
    Card card;
    // Where the card is bought, or nullopt for a loot card.
    std::optional<int> shop_index;
  };

  void BuildFloor();
  std::unique_ptr<Level> MakeLevel(int id, bool is_final_level);
  void StartBattle(bool boss);
  void GrantLootIfWon();

  Difficulty difficulty_;
  Rng rng_;
  Player player_;
  FloorMap map_;
  int floor_ = 1;

  std::vector<std::unique_ptr<Enemy>> enemies_;
  // Declared after enemies_ so it is destroyed first: it points into them.
  std::unique_ptr<Combat> combat_;
  bool boss_battle_ = false;
  bool loot_granted_ = false;
  int loot_gold_ = 0;
  std::vector<Card> loot_cards_;

  std::optional<PendingCard> pending_;
};
