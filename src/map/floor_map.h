#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "level/level.h"
#include "level/level_type.h"

class FloorMap {
 public:
  using LevelFactory =
      std::function<std::unique_ptr<Level>(int, LevelType, bool)>;

  void GenerateMap(const LevelFactory& make_level);
  void DisplayPaths() const;
  std::vector<int> GetAvailablePaths() const;
  bool MoveTo(int id);
  Level* GetCurrentLevel() const;

 private:
  std::vector<std::unique_ptr<Level>> levels_;
  int current_level_id_ = 0;
};
