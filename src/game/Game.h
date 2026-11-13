#pragma once

#include <vector>

#include "map/floor_map.h"
#include "player/player.h"

class Game {
 private:
  std::vector<FloorMap> maps;
  Player player;
  int currentFloor = 0;
  bool gameOver = false;

 public:
  explicit Game(Player player);

  void startGame();
  void run();
  void advanceFloor();
  void endGame();
};
