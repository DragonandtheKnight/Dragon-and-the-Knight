#pragma once

#include <memory>
#include <vector>

#include "level/Level.h"

class FloorMap {
 private:
  std::vector<std::unique_ptr<Level>> levels;
  int currentLevelId = 0;

 public:
  void generateMap();
  void displayPaths() const;
  std::vector<int> getAvailablePaths() const;
  bool moveTo(int id);
  Level* getCurrentLevel() const;
};
