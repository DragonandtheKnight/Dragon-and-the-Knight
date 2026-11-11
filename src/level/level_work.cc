#include <vector>

#include "level/Level.h"
#include "level/LevelType.h"

Level::Level(int id, LevelType type, bool isFinalLevel)
    : id(id), type(type), isFinalLevel(isFinalLevel) {}

void Level::addConnection(int id) {
  connections.push_back(id);
}

std::vector<int> Level::getConnections() const {
  return connections;
}

bool Level::isConnectedTo(int id) const {
  for (int connection_id : connections) {
    if (connection_id == id) {
      return true;
    }
  }
  return false;
}

LevelType Level::getType() const {
  return type;
}
