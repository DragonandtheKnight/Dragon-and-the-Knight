#include "map/floor_map.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "game/rng.h"
#include "level/level.h"
#include "level/level_type.h"

using PathList = std::vector<std::vector<int>>;

static bool HasPath(const PathList& paths, int from, int to) {
  return std::find(paths[from].begin(), paths[from].end(), to) !=
         paths[from].end();
}

static bool HasIncomingPath(const PathList& paths, int to) {
  for (const auto& outgoing : paths) {
    if (std::find(outgoing.begin(), outgoing.end(), to) != outgoing.end()) {
      return true;
    }
  }
  return false;
}

static int Nearest(int from, const std::vector<int>& candidates, Rng& rng) {
  int best_distance = FloorMap::kRows;
  std::vector<int> best;
  for (int id : candidates) {
    const int distance = std::abs(FloorMap::RowOf(id) - FloorMap::RowOf(from));
    if (distance < best_distance) {
      best_distance = distance;
      best = {id};
    } else if (distance == best_distance) {
      best.push_back(id);
    }
  }
  return best[rng.PickIndex(static_cast<int>(best.size()))];
}

int FloorMap::GridId(int column, int row) { return column * kRows + row + 1; }

int FloorMap::ColumnOf(int id) { return (id - 1) / kRows; }

int FloorMap::RowOf(int id) { return (id - 1) % kRows; }

void FloorMap::GenerateMap(const LevelFactory& make_level, Rng& rng) {
  if (!make_level) {
    throw std::invalid_argument("The level factory is empty");
  }

  std::vector<int> order;
  for (int id = GridId(1, 0); id < kBossId; ++id) {
    order.push_back(id);
  }
  rng.Shuffle(order);
  std::vector<bool> removed(kBossId + 1, false);
  std::vector<int> removed_in_column(kColumns, 0);
  int removed_count = 0;
  for (int id : order) {
    if (removed_count == kRemovedLevels) {
      break;
    }
    if (removed_in_column[ColumnOf(id)] == kRows - kMinLevelsPerColumn) {
      continue;
    }
    removed[id] = true;
    ++removed_in_column[ColumnOf(id)];
    ++removed_count;
  }

  std::vector<std::vector<int>> columns(kColumns);
  for (int column = 0; column < kColumns; ++column) {
    for (int row = 0; row < kRows; ++row) {
      if (!removed[GridId(column, row)]) {
        columns[column].push_back(GridId(column, row));
      }
    }
  }

  PathList paths(kBossId + 1);
  paths[kStartId] = columns.front();
  for (int id : columns.back()) {
    paths[id].push_back(kBossId);
  }
  for (int column = 0; column + 1 < kColumns; ++column) {
    const auto& from = columns[column];
    const auto& to = columns[column + 1];
    for (int id : from) {
      paths[id].push_back(Nearest(id, to, rng));
    }
    for (int id : to) {
      if (!HasIncomingPath(paths, id)) {
        paths[Nearest(id, from, rng)].push_back(id);
      }
    }
  }

  std::vector<std::pair<int, int>> near;
  std::vector<std::pair<int, int>> far;
  for (int column = 0; column + 1 < kColumns; ++column) {
    for (int from : columns[column]) {
      for (int to : columns[column + 1]) {
        if (HasPath(paths, from, to)) {
          continue;
        }
        (std::abs(RowOf(from) - RowOf(to)) <= 1 ? near : far)
            .emplace_back(from, to);
      }
    }
  }
  rng.Shuffle(near);
  rng.Shuffle(far);
  near.insert(near.end(), far.begin(), far.end());
  const int wanted = rng.UniformInt(kMinExtraPaths, kMaxExtraPaths);
  const int extra_count = std::min<int>(wanted, near.size());
  std::vector<std::pair<int, int>> new_extra_paths(near.begin(),
                                                   near.begin() + extra_count);
  for (const auto& [from, to] : new_extra_paths) {
    paths[from].push_back(to);
  }

  std::vector<std::pair<int, int>> vertical;
  for (int column = 1; column < kColumns; ++column) {
    for (int row = 0; row + 1 < kRows; ++row) {
      const int upper = GridId(column, row);
      const int lower = GridId(column, row + 1);
      if (!removed[upper] && !removed[lower]) {
        vertical.emplace_back(upper, lower);
      }
    }
  }
  rng.Shuffle(vertical);
  const int vertical_wanted =
      rng.UniformInt(kMinVerticalPaths, kMaxVerticalPaths);
  const int vertical_count = std::min<int>(vertical_wanted, vertical.size());
  std::vector<std::pair<int, int>> new_vertical_paths(
      vertical.begin(), vertical.begin() + vertical_count);
  for (const auto& [upper, lower] : new_vertical_paths) {
    paths[upper].push_back(lower);
    paths[lower].push_back(upper);
  }

  std::vector<std::unique_ptr<Level>> new_levels(kBossId + 1);
  for (int id = kStartId; id <= kBossId; ++id) {
    if (removed[id]) {
      continue;
    }
    auto level = make_level(id, LevelType::kCombat, id == kBossId);
    if (level == nullptr) {
      throw std::invalid_argument("The level factory returned a null level");
    }
    std::sort(paths[id].begin(), paths[id].end());
    for (int to : paths[id]) {
      level->AddConnection(to);
    }
    new_levels[id] = std::move(level);
  }

  levels_ = std::move(new_levels);
  extra_paths_ = std::move(new_extra_paths);
  vertical_paths_ = std::move(new_vertical_paths);
  visited_.assign(kBossId + 1, false);
  visited_[kStartId] = true;
  current_level_id_ = kStartId;
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
  std::vector<int> paths;
  for (int id : current->GetConnections()) {
    if (!IsVisited(id)) {
      paths.push_back(id);
    }
  }
  return paths;
}

bool FloorMap::MoveTo(int id) {
  const Level* current = GetCurrentLevel();
  if (current == nullptr || GetLevel(id) == nullptr ||
      !current->IsConnectedTo(id) || IsVisited(id)) {
    return false;
  }
  current_level_id_ = id;
  visited_[id] = true;
  return true;
}

Level* FloorMap::GetCurrentLevel() const { return GetLevel(current_level_id_); }

Level* FloorMap::GetLevel(int id) const {
  if (id < 0 || id >= static_cast<int>(levels_.size())) {
    return nullptr;
  }
  return levels_[id].get();
}

std::vector<int> FloorMap::GetColumn(int column) const {
  std::vector<int> ids;
  if (column < 0 || column >= kColumns) {
    return ids;
  }
  for (int row = 0; row < kRows; ++row) {
    if (GetLevel(GridId(column, row)) != nullptr) {
      ids.push_back(GridId(column, row));
    }
  }
  return ids;
}

bool FloorMap::IsExtraPath(int from, int to) const {
  return std::find(extra_paths_.begin(), extra_paths_.end(),
                   std::make_pair(from, to)) != extra_paths_.end();
}

bool FloorMap::IsVerticalPath(int from, int to) const {
  const auto pair = from < to ? std::make_pair(from, to)
                              : std::make_pair(to, from);
  return std::find(vertical_paths_.begin(), vertical_paths_.end(), pair) !=
         vertical_paths_.end();
}

bool FloorMap::IsVisited(int id) const {
  return id >= 0 && id < static_cast<int>(visited_.size()) && visited_[id];
}
