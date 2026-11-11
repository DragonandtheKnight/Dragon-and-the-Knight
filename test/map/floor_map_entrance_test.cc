#include <array>
#include <iostream>
#include <memory>
#include <vector>

#include "level/Level.h"
#include "level/LevelType.h"
#include "map/FloorMap.h"

class TestLevel : public Level {
 public:
  using Level::Level;
  void enter(Player&) override {}
};

int main() {
  FloorMap map;
  const std::vector<int> entrances = {1, 2, 3};
  bool passed = true;

  for (int entrance : entrances) {
    std::array<const Level*, 4> nodes{};
    auto make_level = [&nodes](int id, LevelType type,
                               bool is_final) -> std::unique_ptr<Level> {
      auto level = std::make_unique<TestLevel>(id, type, is_final);
      nodes.at(id) = level.get();
      return level;
    };
    map.generateMap(make_level);
    const Level* start = map.getCurrentLevel();
    if (start == nullptr || start != nodes[0] ||
        map.getAvailablePaths() != entrances) {
      std::cerr << "FAIL: the start should offer exactly entrances 1, 2, 3\n";
      return 1;
    }

    const bool moved = map.moveTo(entrance);
    const Level* destination = map.getCurrentLevel();
    if (!moved || destination == nullptr || destination != nodes[entrance]) {
      std::cerr << "FAIL: cannot move from the start to " << entrance << '\n';
      passed = false;
    } else {
      std::cout << "PASS: moved from the start to " << entrance << '\n';
    }
  }

  return passed ? 0 : 1;
}
