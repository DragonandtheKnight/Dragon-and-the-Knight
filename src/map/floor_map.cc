#include "map/floor_map.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "level/level.h"
#include "level/level_type.h"

void FloorMap::GenerateMap(const LevelFactory& make_level) {
  if (!make_level) {
    throw std::invalid_argument("The level factory is empty");
  }

  constexpr int kBossId = 7;
  std::vector<std::unique_ptr<Level>> new_levels;
  for (int id = 0; id <= kBossId; ++id) {
    auto level = make_level(id, LevelType::kCombat, id == kBossId);
    if (level == nullptr) {
      throw std::invalid_argument("The level factory returned a null level");
    }
    new_levels.push_back(std::move(level));
  }

  for (int entrance = 1; entrance <= 3; ++entrance) {
    const int next = entrance + 3;
    new_levels[0]->AddConnection(entrance);
    new_levels[entrance]->AddConnection(next);
    new_levels[next]->AddConnection(kBossId);
  }

  levels_ = std::move(new_levels);
  current_level_id_ = 0;
}

void FloorMap::DisplayPaths() const {
  const auto paths = GetAvailablePaths();
  if (paths.empty()) {
    std::cout << "No available paths\n";
    return;
  }

  std::cout << "Available paths:";
  for (int id : paths) {
    std::cout << ' ' << id;
  }
  std::cout << '\n';
}

std::vector<int> FloorMap::GetAvailablePaths() const {
  const Level* current = GetCurrentLevel();
  if (current == nullptr) {
    return {};
  }
  return current->GetConnections();
}

bool FloorMap::MoveTo(int id) {
  if (id < 0 || id >= static_cast<int>(levels_.size())) {
    return false;
  }

  const Level* current = GetCurrentLevel();
  if (current == nullptr || !current->IsConnectedTo(id)) {
    return false;
  }
  current_level_id_ = id;
  return true;
}

Level* FloorMap::GetCurrentLevel() const {
  if (current_level_id_ < 0 ||
      current_level_id_ >= static_cast<int>(levels_.size())) {
    return nullptr;
  }
  return levels_[current_level_id_].get();
}
