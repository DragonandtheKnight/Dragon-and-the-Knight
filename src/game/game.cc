#include "game/game.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "enemy/dragon.h"
#include "enemy/empty_enemy.h"
#include "enemy/giant.h"
#include "enemy/goblin.h"
#include "enemy/shaman.h"
#include "enemy/slime.h"
#include "game/card_catalog.h"
#include "level/combat_level.h"
#include "level/entertainment.h"
#include "level/event.h"
#include "level/level_type.h"
#include "level/rest.h"

namespace {

constexpr int kRestHeal = 20;
constexpr int kEntertainmentCost = 10;
constexpr int kShopCards = 3;
constexpr int kBossGold = 50;

int MaxHpFor(Difficulty difficulty) {
  switch (difficulty) {
    case Difficulty::kSimple:
      return 100;
    case Difficulty::kNormal:
      return 80;
    case Difficulty::kHard:
      return 60;
  }
  return 80;
}

std::unique_ptr<Enemy> MakeRegularEnemy(int kind, int level, uint32_t seed) {
  switch (kind) {
    case 0:
      return std::make_unique<Slime>(level, seed);
    case 1:
      return std::make_unique<Goblin>(level, seed);
    case 2:
      return std::make_unique<Shaman>(level, seed);
    default:
      return std::make_unique<Giant>(level, seed);
  }
}

}  // namespace

Game::Game(Difficulty difficulty, uint32_t seed)
    : difficulty_(difficulty),
      rng_(seed),
      player_("Knight", MaxHpFor(difficulty), rng_.NextU32()) {
  for (const Card& card : card_catalog::StarterDeck()) {
    player_.GetDeck().AddCard(card);
  }
  BuildFloor();
}

const Player& Game::GetPlayer() const { return player_; }

const FloorMap& Game::GetMap() const { return map_; }

int Game::GetFloor() const { return floor_; }

bool Game::IsFinalFloor() const { return floor_ == kFloors; }

Difficulty Game::GetDifficulty() const { return difficulty_; }

void Game::BuildFloor() {
  map_ = FloorMap();
  map_.GenerateMap(
      [this](int id, LevelType, bool is_final_level) {
        return MakeLevel(id, is_final_level);
      },
      rng_);
}

// The first column is always a battle so every floor opens with one.
std::unique_ptr<Level> Game::MakeLevel(int id, bool is_final_level) {
  if (id == FloorMap::kStartId || is_final_level ||
      FloorMap::ColumnOf(id) == 0) {
    return std::make_unique<CombatLevel>(id, is_final_level);
  }
  const int roll = rng_.UniformInt(1, 100);
  if (roll <= 50) {
    return std::make_unique<CombatLevel>(id, false);
  }
  if (roll <= 65) {
    return std::make_unique<Event>(id, rng_);
  }
  if (roll <= 78) {
    std::vector<Card> cards = card_catalog::RandomRewards(rng_, kShopCards);
    std::vector<int> prices;
    for (const Card& card : cards) {
      prices.push_back(card_catalog::PriceOf(card));
    }
    return std::make_unique<Shop>(id, std::move(cards), std::move(prices));
  }
  if (roll <= 90) {
    return std::make_unique<Rest>(id, kRestHeal);
  }
  return std::make_unique<Entertainment>(id, kEntertainmentCost, rng_);
}

bool Game::EnterLevel(int id) {
  if (combat_ != nullptr || pending_.has_value() || !map_.MoveTo(id)) {
    return false;
  }
  Level* level = map_.GetCurrentLevel();
  if (level->GetType() == LevelType::kCombat) {
    StartBattle(level->IsFinalLevel());
  } else {
    level->Enter(player_);
  }
  return true;
}

const Level* Game::GetCurrentLevel() const { return map_.GetCurrentLevel(); }

void Game::StartBattle(bool boss) {
  combat_.reset();
  enemies_.clear();
  boss_battle_ = boss;
  loot_granted_ = false;
  loot_gold_ = 0;
  loot_cards_.clear();

  if (boss) {
    enemies_.push_back(std::make_unique<Dragon>(floor_, rng_.NextU32()));
  } else {
    const int count = rng_.UniformInt(floor_ == 1 ? 1 : 2, floor_ + 1);
    for (int i = 0; i < count; ++i) {
      enemies_.push_back(
          MakeRegularEnemy(rng_.UniformInt(0, 3), floor_, rng_.NextU32()));
    }
  }
  while (static_cast<int>(enemies_.size()) < Combat::kMaxEnemySlots) {
    enemies_.push_back(std::make_unique<EmptyEnemy>(floor_, 0));
  }

  std::vector<Enemy*> slots;
  for (const std::unique_ptr<Enemy>& enemy : enemies_) {
    slots.push_back(enemy.get());
  }
  combat_ = std::make_unique<Combat>(player_, slots);
  combat_->StartBattle();
}

const Combat* Game::GetCombat() const { return combat_.get(); }

bool Game::IsBossBattle() const { return boss_battle_; }

bool Game::PlayCard(int hand_index, int target_slot) {
  if (GetBattleResult() != BattleResult::kOngoing ||
      !combat_->UseCard(hand_index, target_slot)) {
    return false;
  }
  GrantLootIfWon();
  return true;
}

bool Game::EndTurn() {
  if (GetBattleResult() != BattleResult::kOngoing) {
    return false;
  }
  combat_->EndRound();
  GrantLootIfWon();
  return true;
}

BattleResult Game::GetBattleResult() const {
  if (combat_ == nullptr) {
    return BattleResult::kNone;
  }
  if (!player_.IsAlive()) {
    return BattleResult::kLost;
  }
  return combat_->IsOver() ? BattleResult::kWon : BattleResult::kOngoing;
}

void Game::GrantLootIfWon() {
  if (loot_granted_ || GetBattleResult() != BattleResult::kWon) {
    return;
  }
  loot_granted_ = true;
  loot_gold_ = boss_battle_ ? kBossGold : rng_.UniformInt(10, 20) + 5 * floor_;
  player_.GainGold(loot_gold_);
  loot_cards_ = card_catalog::RandomRewards(rng_, kLootChoices);
}

int Game::GetLootGold() const { return loot_gold_; }

const std::vector<Card>& Game::GetLootCards() const { return loot_cards_; }

bool Game::PickLootCard(int index) {
  if (!loot_granted_ || pending_.has_value() || index < 0 ||
      index >= static_cast<int>(loot_cards_.size())) {
    return false;
  }
  pending_ = PendingCard{loot_cards_[index], std::nullopt};
  return true;
}

void Game::FinishLoot() {
  if (!loot_granted_) {
    return;
  }
  pending_.reset();
  combat_.reset();
  enemies_.clear();
  loot_cards_.clear();
  loot_granted_ = false;
  player_.ResetShield();
}

const Shop* Game::GetShop() const {
  const Level* level = map_.GetCurrentLevel();
  if (level == nullptr || level->GetType() != LevelType::kShop) {
    return nullptr;
  }
  return static_cast<const Shop*>(level);
}

bool Game::PickShopCard(int index) {
  const Shop* shop = GetShop();
  if (shop == nullptr || pending_.has_value() || index < 0 ||
      index >= static_cast<int>(shop->GetCards().size()) ||
      player_.GetGold() < shop->GetPrices()[index]) {
    return false;
  }
  pending_ = PendingCard{shop->GetCards()[index], index};
  return true;
}

bool Game::BuyHeal() {
  if (GetShop() == nullptr || player_.GetHp() == player_.GetMaxHp() ||
      !player_.SpendGold(kHealPrice)) {
    return false;
  }
  player_.Heal(kHealAmount);
  return true;
}

const Card* Game::GetPendingCard() const {
  return pending_.has_value() ? &pending_->card : nullptr;
}

bool Game::ConfirmExchange(int deck_index) {
  Deck& deck = player_.GetDeck();
  if (!pending_.has_value() || deck_index < 0 ||
      deck_index >= static_cast<int>(deck.GetCards().size())) {
    return false;
  }
  if (pending_->shop_index.has_value()) {
    Shop* shop = static_cast<Shop*>(map_.GetCurrentLevel());
    if (!shop->ConfirmExchange(player_, *pending_->shop_index, deck_index)) {
      return false;
    }
  } else {
    deck.RemoveCard(deck_index);
    deck.AddCard(pending_->card);
    loot_cards_.clear();
  }
  pending_.reset();
  return true;
}

void Game::CancelExchange() { pending_.reset(); }

bool Game::ChooseEventOption(int choice) {
  Level* level = map_.GetCurrentLevel();
  if (level == nullptr || level->GetType() != LevelType::kEvent) {
    return false;
  }
  Event* event = static_cast<Event*>(level);
  if (choice < 0 || choice >= static_cast<int>(event->GetChoices().size())) {
    return false;
  }
  event->ApplyChoice(player_, choice);
  return true;
}

bool Game::NextFloor() {
  if (IsFinalFloor() || combat_ != nullptr) {
    return false;
  }
  ++floor_;
  BuildFloor();
  return true;
}
