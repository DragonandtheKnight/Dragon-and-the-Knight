#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "level/Level.h"
#include "level/LevelType.h"

class FloorMap {
 private:
  std::vector<std::unique_ptr<Level>> levels;
  int currentLevelId = 0;

 public:
  using LevelFactory =
      std::function<std::unique_ptr<Level>(int, LevelType, bool)>;

  void generateMap(const LevelFactory& make_level);
  void displayPaths() const;
  std::vector<int> getAvailablePaths() const;
  bool moveTo(int id);
  Level* getCurrentLevel() const;
};
