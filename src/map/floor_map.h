#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "game/rng.h"
#include "level/level.h"
#include "level/level_type.h"

class FloorMap {
 public:
  using LevelFactory =
      std::function<std::unique_ptr<Level>(int, LevelType, bool)>;

  static constexpr int kRows = 4;
  static constexpr int kColumns = 5;
  static constexpr int kRemovedLevels = 5;
  static constexpr int kMinLevelsPerColumn = 2;
  static constexpr int kMinExtraPaths = 1;
  static constexpr int kMaxExtraPaths = 3;
  static constexpr int kMinVerticalPaths = 1;
  static constexpr int kMaxVerticalPaths = 3;
  static constexpr int kStartId = 0;
  static constexpr int kBossId = kRows * kColumns + 1;

  void GenerateMap(const LevelFactory& make_level, Rng& rng);
  void DisplayPaths() const;
  std::vector<int> GetAvailablePaths() const;
  bool MoveTo(int id);
  Level* GetCurrentLevel() const;

  Level* GetLevel(int id) const;
  std::vector<int> GetColumn(int column) const;
  bool IsExtraPath(int from, int to) const;
  bool IsVerticalPath(int from, int to) const;
  bool IsVisited(int id) const;

  static int GridId(int column, int row);
  static int ColumnOf(int id);
  static int RowOf(int id);

 private:
  std::vector<std::unique_ptr<Level>> levels_;
  std::vector<std::pair<int, int>> extra_paths_;
  std::vector<std::pair<int, int>> vertical_paths_;
  std::vector<bool> visited_;
  int current_level_id_ = kStartId;
};
