#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"
#include "level/level.h"
#include "level/level_type.h"
#include "map/floor_map.h"

class TestLevel : public Level {
 public:
  using Level::Level;
  void Enter(Player&) override {}
};

static std::unique_ptr<Level> MakeTestLevel(int id, LevelType type,
                                            bool is_final) {
  return std::make_unique<TestLevel>(id, type, is_final);
}

TEST(LevelTest, ReportsIdTypeAndFinalFlag) {
  const TestLevel level(3, LevelType::kShop, true);
  EXPECT_EQ(level.GetId(), 3);
  EXPECT_EQ(level.GetType(), LevelType::kShop);
  EXPECT_TRUE(level.IsFinalLevel());
}

TEST(LevelTest, RecordsAndLooksUpOutgoingConnections) {
  TestLevel level(1, LevelType::kCombat, false);
  EXPECT_TRUE(level.GetConnections().empty());
  EXPECT_FALSE(level.IsConnectedTo(4));

  level.AddConnection(4);
  EXPECT_EQ(level.GetConnections(), std::vector<int>{4});
  EXPECT_TRUE(level.IsConnectedTo(4));
  EXPECT_FALSE(level.IsConnectedTo(1));
}

TEST(FloorMapTest, IsSafeBeforeGeneration) {
  FloorMap map;
  EXPECT_EQ(map.GetCurrentLevel(), nullptr);
  EXPECT_TRUE(map.GetAvailablePaths().empty());
  EXPECT_FALSE(map.MoveTo(0));

  std::ostringstream output;
  auto* original = std::cout.rdbuf(output.rdbuf());
  map.DisplayPaths();
  std::cout.rdbuf(original);
  EXPECT_EQ(output.str(), "No available paths\n");
}

TEST(FloorMapTest, DisplaysAvailablePaths) {
  FloorMap map;
  map.GenerateMap(MakeTestLevel);

  std::ostringstream output;
  auto* original = std::cout.rdbuf(output.rdbuf());
  map.DisplayPaths();
  std::cout.rdbuf(original);
  EXPECT_EQ(output.str(), "Available paths: 1 2 3\n");
}

TEST(FloorMapTest, AllThreeRoutesReachTheOnlyFinalLevel) {
  const std::array<std::array<int, 6>, 3> routes = {{
      {1, 4, 7, 10, 13, 16},
      {2, 5, 8, 11, 14, 16},
      {3, 6, 9, 12, 15, 16},
  }};

  auto check_route = [](const auto& route) {
    SCOPED_TRACE(route.front());
    FloorMap map;
    map.GenerateMap(MakeTestLevel);

    ASSERT_NE(map.GetCurrentLevel(), nullptr);
    EXPECT_EQ(map.GetCurrentLevel()->GetId(), 0);
    EXPECT_FALSE(map.GetCurrentLevel()->IsFinalLevel());
    EXPECT_EQ(map.GetAvailablePaths(), (std::vector<int>{1, 2, 3}));

    for (auto step = route.begin(); step != route.end(); ++step) {
      const int destination = *step;
      SCOPED_TRACE(destination);
      if (step != route.begin()) {
        EXPECT_EQ(map.GetAvailablePaths(), std::vector<int>{destination});
      }
      ASSERT_TRUE(map.MoveTo(destination));
      ASSERT_NE(map.GetCurrentLevel(), nullptr);
      EXPECT_EQ(map.GetCurrentLevel()->GetId(), destination);
      EXPECT_EQ(map.GetCurrentLevel()->IsFinalLevel(), step + 1 == route.end());
    }
    EXPECT_TRUE(map.GetAvailablePaths().empty());
  };

  for (const auto& route : routes) {
    check_route(route);
  }
}

TEST(FloorMapTest, RejectsInvalidAndUnconnectedMoves) {
  FloorMap map;
  map.GenerateMap(MakeTestLevel);
  ASSERT_TRUE(map.MoveTo(1));
  const Level* current = map.GetCurrentLevel();

  for (int id : {-1, 17, 2, 0}) {
    SCOPED_TRACE(id);
    EXPECT_FALSE(map.MoveTo(id));
    EXPECT_EQ(map.GetCurrentLevel(), current);
  }
}

TEST(FloorMapTest, RejectsAnEmptyFactory) {
  FloorMap map;
  EXPECT_THROW(map.GenerateMap({}), std::invalid_argument);
  EXPECT_EQ(map.GetCurrentLevel(), nullptr);
}

TEST(FloorMapTest, RejectsANullLevelWithoutChangingTheExistingMap) {
  FloorMap map;
  map.GenerateMap(MakeTestLevel);
  ASSERT_TRUE(map.MoveTo(1));
  const Level* current = map.GetCurrentLevel();

  auto incomplete_factory = [](int id, LevelType type,
                               bool is_final) -> std::unique_ptr<Level> {
    if (id == 4) {
      return nullptr;
    }
    return MakeTestLevel(id, type, is_final);
  };
  EXPECT_THROW(map.GenerateMap(incomplete_factory), std::invalid_argument);
  EXPECT_EQ(map.GetCurrentLevel(), current);
  EXPECT_EQ(map.GetAvailablePaths(), std::vector<int>{4});
}

TEST(FloorMapTest, RegenerationReturnsToTheStart) {
  FloorMap map;
  map.GenerateMap(MakeTestLevel);
  ASSERT_TRUE(map.MoveTo(2));

  map.GenerateMap(MakeTestLevel);

  ASSERT_NE(map.GetCurrentLevel(), nullptr);
  EXPECT_EQ(map.GetCurrentLevel()->GetId(), 0);
  EXPECT_EQ(map.GetAvailablePaths(), (std::vector<int>{1, 2, 3}));
}
