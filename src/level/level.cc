#include "level/level.h"

#include <algorithm>
#include <vector>

#include "level/level_type.h"

Level::Level(int id, LevelType type, bool is_final_level)
    : id_(id), type_(type), is_final_level_(is_final_level) {}

int Level::GetId() const { return id_; }

LevelType Level::GetType() const { return type_; }

bool Level::IsFinalLevel() const { return is_final_level_; }

std::vector<int> Level::GetConnections() const { return connections_; }

void Level::AddConnection(int id) { connections_.push_back(id); }

bool Level::IsConnectedTo(int id) const {
  return std::find(connections_.begin(), connections_.end(), id) !=
         connections_.end();
}
