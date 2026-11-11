#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "level/Level.h"
#include "level/LevelType.h"
#include "map/FloorMap.h"

Level* FloorMap::getCurrentLevel() const {
  if (currentLevelId < 0 || currentLevelId >= static_cast<int>(levels.size())) {
    return nullptr;
  }
  return levels[currentLevelId].get();
}

void FloorMap::generateMap(const LevelFactory& make_level) {
  std::vector<std::unique_ptr<Level>> new_levels;
  for (int id = 0; id < 4; ++id) {
    auto level = make_level(id, LevelType::Combat, id != 0);
    if (level == nullptr) {
      throw std::invalid_argument("The level factory returned a null level");
    }
    new_levels.push_back(std::move(level));
  }

  new_levels[0]->addConnection(1);
  new_levels[0]->addConnection(2);
  new_levels[0]->addConnection(3);
  levels = std::move(new_levels);
  currentLevelId = 0;
}

std::vector<int> FloorMap::getAvailablePaths() const {
  const Level* current = getCurrentLevel();
  if (current == nullptr) {
    return {};
  }
  return current->getConnections();
}

bool FloorMap::moveTo(int id) {
  if (id < 0 || id >= static_cast<int>(levels.size())) {
    return false;
  }
  const Level* current = getCurrentLevel();
  if (current == nullptr || !current->isConnectedTo(id)) {
    return false;
  }
  currentLevelId = id;
  return true;
}
