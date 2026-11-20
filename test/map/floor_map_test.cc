#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "game/rng.h"
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

static FloorMap MakeMap(uint32_t seed) {
  FloorMap map;
  Rng rng(seed);
  map.GenerateMap(MakeTestLevel, rng);
  return map;
}

static std::set<std::string> AllPaths(const FloorMap& map) {
  std::set<std::string> paths;
  for (int id = FloorMap::kStartId; id <= FloorMap::kBossId; ++id) {
    const Level* level = map.GetLevel(id);
    if (level == nullptr) {
      continue;
    }
    for (int to : level->GetConnections()) {
      paths.insert(std::to_string(id) + ">" + std::to_string(to));
    }
  }
  return paths;
}

static std::vector<int> RemainingGridLevels(const FloorMap& map) {
  std::vector<int> ids;
  for (int column = 0; column < FloorMap::kColumns; ++column) {
    for (int id : map.GetColumn(column)) {
      ids.push_back(id);
    }
  }
  return ids;
}

static std::set<int> ReachableFrom(const FloorMap& map, int start) {
  std::set<int> seen = {start};
  std::vector<int> pending = {start};
  while (!pending.empty()) {
    const int id = pending.back();
    pending.pop_back();
    for (int to : map.GetLevel(id)->GetConnections()) {
      if (seen.insert(to).second) {
        pending.push_back(to);
      }
    }
  }
  return seen;
}

constexpr int kSeedsToCheck = 300;

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

TEST(FloorMapTest, GridIdsRoundTrip) {
  EXPECT_EQ(FloorMap::GridId(0, 0), 1);
  EXPECT_EQ(FloorMap::GridId(4, 3), 20);
  EXPECT_EQ(FloorMap::kBossId, 21);
  for (int column = 0; column < FloorMap::kColumns; ++column) {
    for (int row = 0; row < FloorMap::kRows; ++row) {
      const int id = FloorMap::GridId(column, row);
      EXPECT_EQ(FloorMap::ColumnOf(id), column);
      EXPECT_EQ(FloorMap::RowOf(id), row);
    }
  }
}

TEST(FloorMapTest, IsSafeBeforeGeneration) {
  FloorMap map;
  EXPECT_EQ(map.GetCurrentLevel(), nullptr);
  EXPECT_TRUE(map.GetAvailablePaths().empty());
  EXPECT_FALSE(map.MoveTo(0));
  EXPECT_TRUE(map.GetColumn(0).empty());

  std::ostringstream output;
  auto* original = std::cout.rdbuf(output.rdbuf());
  map.DisplayPaths();
  std::cout.rdbuf(original);
  EXPECT_EQ(output.str(), "No available paths\n");
}

TEST(FloorMapTest, StartsAtTheStartWithTheFirstColumnAsExits) {
  const FloorMap map = MakeMap(7);
  ASSERT_NE(map.GetCurrentLevel(), nullptr);
  EXPECT_EQ(map.GetCurrentLevel()->GetId(), FloorMap::kStartId);
  EXPECT_FALSE(map.GetCurrentLevel()->IsFinalLevel());
  EXPECT_EQ(map.GetAvailablePaths(), map.GetColumn(0));

  std::ostringstream expected;
  expected << "Available paths:";
  for (int id : map.GetColumn(0)) {
    expected << ' ' << id;
  }
  expected << '\n';
  std::ostringstream output;
  auto* original = std::cout.rdbuf(output.rdbuf());
  map.DisplayPaths();
  std::cout.rdbuf(original);
  EXPECT_EQ(output.str(), expected.str());
}

TEST(FloorMapTest, TheSameSeedGivesTheSameMap) {
  for (uint32_t seed = 0; seed < 20; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap first = MakeMap(seed);
    const FloorMap second = MakeMap(seed);
    EXPECT_EQ(RemainingGridLevels(first), RemainingGridLevels(second));
    EXPECT_EQ(AllPaths(first), AllPaths(second));
  }
}

TEST(FloorMapTest, DifferentSeedsGiveDifferentMaps) {
  std::set<std::set<std::string>> layouts;
  for (uint32_t seed = 0; seed < 20; ++seed) {
    layouts.insert(AllPaths(MakeMap(seed)));
  }
  EXPECT_GT(layouts.size(), 1u);
}

TEST(FloorMapTest, RemovesFiveLevelsAndKeepsEnoughInEveryColumn) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    EXPECT_EQ(RemainingGridLevels(map).size(),
              static_cast<size_t>(FloorMap::kRows * FloorMap::kColumns -
                                  FloorMap::kRemovedLevels));
    EXPECT_EQ(map.GetColumn(0).size(), static_cast<size_t>(FloorMap::kRows));
    for (int column = 0; column < FloorMap::kColumns; ++column) {
      EXPECT_GE(map.GetColumn(column).size(),
                static_cast<size_t>(FloorMap::kMinLevelsPerColumn))
          << "column " << column;
    }
    ASSERT_NE(map.GetLevel(FloorMap::kStartId), nullptr);
    ASSERT_NE(map.GetLevel(FloorMap::kBossId), nullptr);
    EXPECT_TRUE(map.GetLevel(FloorMap::kBossId)->IsFinalLevel());
  }
}

TEST(FloorMapTest, PathsGoToTheNextColumnOrAlongAVerticalPath) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    EXPECT_EQ(map.GetLevel(FloorMap::kStartId)->GetConnections(),
              map.GetColumn(0));
    for (int id : RemainingGridLevels(map)) {
      const int column = FloorMap::ColumnOf(id);
      const auto exits = map.GetLevel(id)->GetConnections();
      ASSERT_FALSE(exits.empty()) << "level " << id;
      for (int to : exits) {
        if (map.IsVerticalPath(id, to)) {
          ASSERT_NE(map.GetLevel(to), nullptr);
          EXPECT_EQ(FloorMap::ColumnOf(to), column);
          EXPECT_EQ(std::abs(FloorMap::RowOf(to) - FloorMap::RowOf(id)), 1);
        } else if (column == FloorMap::kColumns - 1) {
          EXPECT_EQ(to, FloorMap::kBossId);
        } else {
          ASSERT_NE(map.GetLevel(to), nullptr);
          EXPECT_EQ(FloorMap::ColumnOf(to), column + 1);
        }
      }
    }
    EXPECT_TRUE(map.GetLevel(FloorMap::kBossId)->GetConnections().empty());
  }
}

TEST(FloorMapTest, EveryLevelIsReachableAndReachesTheBoss) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    const auto from_start = ReachableFrom(map, FloorMap::kStartId);
    for (int id : RemainingGridLevels(map)) {
      EXPECT_EQ(from_start.count(id), 1u) << "level " << id;
      EXPECT_EQ(ReachableFrom(map, id).count(FloorMap::kBossId), 1u)
          << "level " << id;
    }
  }
}

TEST(FloorMapTest, BasePathsAloneKeepEveryLevelOnARoute) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    for (int id : RemainingGridLevels(map)) {
      int base_exits = 0;
      for (int to : map.GetLevel(id)->GetConnections()) {
        base_exits += !map.IsExtraPath(id, to) && !map.IsVerticalPath(id, to);
      }
      EXPECT_GE(base_exits, 1) << "level " << id;
      if (FloorMap::ColumnOf(id) == 0) {
        continue;
      }
      int base_entrances = 0;
      for (int from : map.GetColumn(FloorMap::ColumnOf(id) - 1)) {
        base_entrances += map.GetLevel(from)->IsConnectedTo(id) &&
                          !map.IsExtraPath(from, id);
      }
      EXPECT_GE(base_entrances, 1) << "level " << id;
    }
  }
}

TEST(FloorMapTest, AddsOneToThreeExtraPaths) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    int extra = 0;
    for (int id : RemainingGridLevels(map)) {
      for (int to : map.GetLevel(id)->GetConnections()) {
        extra += map.IsExtraPath(id, to);
      }
    }
    EXPECT_GE(extra, FloorMap::kMinExtraPaths);
    EXPECT_LE(extra, FloorMap::kMaxExtraPaths);
  }
}

TEST(FloorMapTest, MostMapsHaveAForkMidway) {
  int maps_with_fork = 0;
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    const FloorMap map = MakeMap(seed);
    for (int column = 1; column + 1 < FloorMap::kColumns; ++column) {
      bool found = false;
      for (int id : map.GetColumn(column)) {
        found = found || map.GetLevel(id)->GetConnections().size() >= 2;
      }
      if (found) {
        ++maps_with_fork;
        break;
      }
    }
  }
  EXPECT_GT(maps_with_fork, kSeedsToCheck * 3 / 4);
}

TEST(FloorMapTest, WalksFromTheStartToTheBoss) {
  FloorMap map = MakeMap(42);
  int steps = 0;
  while (!map.GetCurrentLevel()->IsFinalLevel()) {
    const auto exits = map.GetAvailablePaths();
    ASSERT_FALSE(exits.empty());
    ASSERT_TRUE(map.MoveTo(exits.back()));
    EXPECT_EQ(map.GetCurrentLevel()->GetId(), exits.back());
    ++steps;
  }
  EXPECT_EQ(steps, FloorMap::kColumns + 1);
  EXPECT_TRUE(map.GetAvailablePaths().empty());
}

TEST(FloorMapTest, AddsOneToThreeTwoWayVerticalPaths) {
  for (uint32_t seed = 0; seed < kSeedsToCheck; ++seed) {
    SCOPED_TRACE(seed);
    const FloorMap map = MakeMap(seed);
    int vertical = 0;
    for (int id : RemainingGridLevels(map)) {
      for (int to : map.GetLevel(id)->GetConnections()) {
        if (!map.IsVerticalPath(id, to)) {
          continue;
        }
        EXPECT_GE(FloorMap::ColumnOf(id), 1) << "level " << id;
        EXPECT_TRUE(map.GetLevel(to)->IsConnectedTo(id))
            << id << " and " << to << " should be linked both ways";
        vertical += id < to;
      }
    }
    EXPECT_GE(vertical, FloorMap::kMinVerticalPaths);
    EXPECT_LE(vertical, FloorMap::kMaxVerticalPaths);
  }
}

static std::vector<int> RouteTo(const FloorMap& map, int id) {
  std::vector<int> parent(FloorMap::kBossId + 1, -1);
  std::vector<int> pending = {FloorMap::kStartId};
  parent[FloorMap::kStartId] = FloorMap::kStartId;
  for (size_t i = 0; i < pending.size(); ++i) {
    for (int to : map.GetLevel(pending[i])->GetConnections()) {
      if (parent[to] == -1) {
        parent[to] = pending[i];
        pending.push_back(to);
      }
    }
  }
  std::vector<int> route;
  for (int step = id; step != FloorMap::kStartId; step = parent[step]) {
    route.insert(route.begin(), step);
  }
  return route;
}

TEST(FloorMapTest, CannotReenterAVisitedLevel) {
  for (uint32_t seed = 0; seed < 20; ++seed) {
    SCOPED_TRACE(seed);
    FloorMap map = MakeMap(seed);
    int upper = -1;
    int lower = -1;
    for (int id : RemainingGridLevels(map)) {
      if (map.IsVerticalPath(id, id + 1)) {
        upper = id;
        lower = id + 1;
        break;
      }
    }
    ASSERT_NE(upper, -1);

    for (int step : RouteTo(map, upper)) {
      ASSERT_TRUE(map.MoveTo(step));
    }
    EXPECT_TRUE(map.IsVisited(upper));
    ASSERT_TRUE(map.MoveTo(lower));

    const auto exits = map.GetAvailablePaths();
    EXPECT_EQ(std::count(exits.begin(), exits.end(), upper), 0);
    EXPECT_TRUE(map.GetCurrentLevel()->IsConnectedTo(upper));
    EXPECT_FALSE(map.MoveTo(upper));
    EXPECT_EQ(map.GetCurrentLevel()->GetId(), lower);
  }
}

TEST(FloorMapTest, RejectsInvalidAndUnconnectedMoves) {
  FloorMap map = MakeMap(3);
  const int first = map.GetAvailablePaths().front();
  ASSERT_TRUE(map.MoveTo(first));
  const Level* current = map.GetCurrentLevel();

  std::vector<int> bad_moves = {-1, FloorMap::kBossId + 1, FloorMap::kStartId,
                                first, FloorMap::kBossId};
  for (int id = 1; id < FloorMap::kBossId; ++id) {
    if (!current->IsConnectedTo(id)) {
      bad_moves.push_back(id);
    }
  }
  for (int id : bad_moves) {
    SCOPED_TRACE(id);
    EXPECT_FALSE(map.MoveTo(id));
    EXPECT_EQ(map.GetCurrentLevel(), current);
  }
}

TEST(FloorMapTest, RejectsAnEmptyFactory) {
  FloorMap map;
  Rng rng(1);
  EXPECT_THROW(map.GenerateMap({}, rng), std::invalid_argument);
  EXPECT_EQ(map.GetCurrentLevel(), nullptr);
}

TEST(FloorMapTest, RejectsANullLevelWithoutChangingTheExistingMap) {
  FloorMap map = MakeMap(5);
  const int first = map.GetAvailablePaths().front();
  ASSERT_TRUE(map.MoveTo(first));
  const Level* current = map.GetCurrentLevel();
  const auto exits = map.GetAvailablePaths();
  const auto paths = AllPaths(map);

  auto incomplete_factory = [](int id, LevelType type,
                               bool is_final) -> std::unique_ptr<Level> {
    if (id == FloorMap::kBossId) {
      return nullptr;
    }
    return MakeTestLevel(id, type, is_final);
  };
  Rng rng(6);
  EXPECT_THROW(map.GenerateMap(incomplete_factory, rng),
               std::invalid_argument);
  EXPECT_EQ(map.GetCurrentLevel(), current);
  EXPECT_EQ(map.GetAvailablePaths(), exits);
  EXPECT_EQ(AllPaths(map), paths);
}

TEST(FloorMapTest, RegenerationReturnsToTheStart) {
  FloorMap map = MakeMap(8);
  const int first = map.GetAvailablePaths().front();
  ASSERT_TRUE(map.MoveTo(first));

  Rng rng(9);
  map.GenerateMap(MakeTestLevel, rng);

  EXPECT_FALSE(map.IsVisited(first));
  ASSERT_NE(map.GetCurrentLevel(), nullptr);
  EXPECT_EQ(map.GetCurrentLevel()->GetId(), FloorMap::kStartId);
  EXPECT_EQ(map.GetAvailablePaths(), map.GetColumn(0));
}
