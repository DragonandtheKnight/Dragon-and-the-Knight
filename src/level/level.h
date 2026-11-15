#pragma once

#include <vector>

#include "level/level_type.h"

class Player;

class Level {
 public:
  Level(int id, LevelType type, bool is_final_level);
  virtual ~Level() = default;

  virtual void Enter(Player& player) = 0;
  int GetId() const;
  LevelType GetType() const;
  bool IsFinalLevel() const;
  std::vector<int> GetConnections() const;
  void AddConnection(int id);
  bool IsConnectedTo(int id) const;

 private:
  int id_;
  LevelType type_;
  std::vector<int> connections_;
  bool is_final_level_;
};
