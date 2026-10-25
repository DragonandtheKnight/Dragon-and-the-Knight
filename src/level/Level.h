#pragma once

#include <vector>

#include "level/LevelType.h"

class Player;

class Level {
 protected:
  int id;
  LevelType type;
  std::vector<int> connections;
  bool isFinalLevel;

 public:
  Level(int id, LevelType type, bool isFinalLevel);
  virtual ~Level() = default;

  virtual void enter(Player& player) = 0;

  LevelType getType() const;
  std::vector<int> getConnections() const;
  void addConnection(int id);
  bool isConnectedTo(int id) const;
};
