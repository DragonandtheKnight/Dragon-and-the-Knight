// Tests for Rng, the game's single seeded random stream (Class UML board,
// "RNG · one seeded stream"). A test that counts outcomes uses a fixed seed,
// so it gives the same result on every run; its bounds are about six standard
// deviations wide, so any correct generator passes.

#include "game/rng.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"

std::vector<uint32_t> FirstValues(Rng& rng, int count) {
  std::vector<uint32_t> values;
  for (int i = 0; i < count; ++i) {
    values.push_back(rng.NextU32());
  }
  return values;
}

std::vector<int> ZeroToNineteen() {
  std::vector<int> numbers;
  for (int i = 0; i < 20; ++i) {
    numbers.push_back(i);
  }
  return numbers;
}

TEST(RngSeedTest, SameSeedGivesSameSequence) {
  Rng first(42);
  Rng second(42);

  EXPECT_EQ(FirstValues(first, 100), FirstValues(second, 100));
}

TEST(RngSeedTest, DifferentSeedsGiveDifferentSequences) {
  Rng first(42);
  Rng second(43);

  EXPECT_NE(FirstValues(first, 10), FirstValues(second, 10));
}

// Pins the engine to std::mt19937, so a seed gives the same run on macOS,
// Linux (CI) and Windows. 5489 is mt19937's default seed, and the C++
// standard requires its 10000th value to be 4123659995.
TEST(RngSeedTest, NextU32MatchesMt19937ReferenceValues) {
  Rng rng(5489);
  const std::vector<uint32_t> values = FirstValues(rng, 10000);

  EXPECT_EQ(values.front(), 3499211612u);
  EXPECT_EQ(values.back(), 4123659995u);
}

// Load Game restores a saved run this way.
TEST(RngStateTest, RestoredStateContinuesTheSameSequence) {
  Rng original(42);
  FirstValues(original, 5);
  Rng restored(7);
  restored.SetState(original.GetState());

  EXPECT_EQ(FirstValues(restored, 20), FirstValues(original, 20));
}

TEST(RngStateTest, RestoringAnEarlierStateReplaysIt) {
  Rng rng(42);
  const std::string saved = rng.GetState();
  const std::vector<uint32_t> first_time = FirstValues(rng, 5);
  rng.SetState(saved);

  EXPECT_EQ(FirstValues(rng, 5), first_time);
}

// A damaged save file must not leave the game with a half-restored engine.
TEST(RngStateTest, RestoringTextThatIsNotAStateThrows) {
  Rng rng(42);

  EXPECT_THROW(rng.SetState("not a saved state"), std::invalid_argument);
  EXPECT_THROW(rng.SetState(""), std::invalid_argument);
}

TEST(RngUniformIntTest, StaysWithinInclusiveRange) {
  Rng rng(42);
  for (int i = 0; i < 1000; ++i) {
    const int value = rng.UniformInt(-3, 3);
    EXPECT_GE(value, -3);
    EXPECT_LE(value, 3);
  }
}

TEST(RngUniformIntTest, ReachesBothEnds) {
  Rng rng(42);
  bool saw_min = false;
  bool saw_max = false;
  for (int i = 0; i < 1000; ++i) {
    const int value = rng.UniformInt(1, 6);
    saw_min = saw_min || value == 1;
    saw_max = saw_max || value == 6;
  }

  EXPECT_TRUE(saw_min);
  EXPECT_TRUE(saw_max);
}

TEST(RngUniformIntTest, EqualMinAndMaxReturnsThatValue) {
  Rng rng(42);
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(rng.UniformInt(5, 5), 5);
  }
}

TEST(RngUniformIntTest, IsRoughlyUniform) {
  Rng rng(42);
  std::map<int, int> counts;
  for (int i = 0; i < 60000; ++i) {
    ++counts[rng.UniformInt(1, 6)];
  }

  ASSERT_EQ(counts.size(), 6u);
  for (const auto& [value, count] : counts) {
    EXPECT_NEAR(count, 10000, 600) << "value " << value;
  }
}

// Computing max - min + 1 as an int overflows here.
TEST(RngUniformIntTest, FullIntRangeDoesNotOverflow) {
  Rng rng(42);
  bool saw_negative = false;
  bool saw_non_negative = false;
  for (int i = 0; i < 1000; ++i) {
    const int value = rng.UniformInt(INT_MIN, INT_MAX);
    saw_negative = saw_negative || value < 0;
    saw_non_negative = saw_non_negative || value >= 0;
  }

  EXPECT_TRUE(saw_negative);
  EXPECT_TRUE(saw_non_negative);
}

TEST(RngUniformIntTest, MinAboveMaxThrows) {
  Rng rng(42);

  EXPECT_THROW(rng.UniformInt(6, 1), std::invalid_argument);
}

TEST(RngChanceTest, ZeroPercentNeverHappens) {
  Rng rng(42);
  for (int i = 0; i < 1000; ++i) {
    EXPECT_FALSE(rng.Chance(0));
  }
}

TEST(RngChanceTest, HundredPercentAlwaysHappens) {
  Rng rng(42);
  for (int i = 0; i < 1000; ++i) {
    EXPECT_TRUE(rng.Chance(100));
  }
}

TEST(RngChanceTest, ThirtyPercentHappensAboutThirtyPercentOfTheTime) {
  Rng rng(42);
  int hits = 0;
  for (int i = 0; i < 10000; ++i) {
    hits += rng.Chance(30) ? 1 : 0;
  }

  EXPECT_NEAR(hits, 3000, 300);
}

TEST(RngChanceTest, PercentOutsideZeroToHundredThrows) {
  Rng rng(42);

  EXPECT_THROW(rng.Chance(-1), std::invalid_argument);
  EXPECT_THROW(rng.Chance(101), std::invalid_argument);
}

TEST(RngPickIndexTest, ReachesEveryIndexAndNoOther) {
  Rng rng(42);
  std::map<int, int> counts;
  for (int i = 0; i < 1000; ++i) {
    ++counts[rng.PickIndex(4)];
  }

  EXPECT_EQ(counts.size(), 4u);
  EXPECT_EQ(counts.begin()->first, 0);
  EXPECT_EQ(counts.rbegin()->first, 3);
}

TEST(RngPickIndexTest, SizeOfOneAlwaysPicksZero) {
  Rng rng(42);
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(rng.PickIndex(1), 0);
  }
}

TEST(RngPickIndexTest, EmptyOrNegativeSizeThrows) {
  Rng rng(42);

  EXPECT_THROW(rng.PickIndex(0), std::invalid_argument);
  EXPECT_THROW(rng.PickIndex(-1), std::invalid_argument);
}

TEST(RngShuffleTest, KeepsTheSameElements) {
  Rng rng(42);
  std::vector<int> numbers = ZeroToNineteen();
  rng.Shuffle(numbers);
  std::sort(numbers.begin(), numbers.end());

  EXPECT_EQ(numbers, ZeroToNineteen());
}

TEST(RngShuffleTest, ChangesTheOrder) {
  Rng rng(42);
  std::vector<int> numbers = ZeroToNineteen();
  rng.Shuffle(numbers);

  EXPECT_NE(numbers, ZeroToNineteen());
}

TEST(RngShuffleTest, SameSeedGivesSameOrder) {
  Rng first(42);
  Rng second(42);
  std::vector<int> first_numbers = ZeroToNineteen();
  std::vector<int> second_numbers = ZeroToNineteen();
  first.Shuffle(first_numbers);
  second.Shuffle(second_numbers);

  EXPECT_EQ(first_numbers, second_numbers);
}

TEST(RngShuffleTest, EmptyAndSingleElementVectorsAreFine) {
  Rng rng(42);
  std::vector<int> empty;
  std::vector<int> one = {7};
  rng.Shuffle(empty);
  rng.Shuffle(one);

  EXPECT_TRUE(empty.empty());
  EXPECT_EQ(one, std::vector<int>{7});
}

// A common shuffle bug swaps each element with any position instead of a
// position not yet visited. That makes some orders about 25% more likely.
TEST(RngShuffleTest, EveryOrderIsEquallyLikely) {
  Rng rng(42);
  std::map<std::vector<int>, int> counts;
  for (int i = 0; i < 60000; ++i) {
    std::vector<int> numbers = {0, 1, 2};
    rng.Shuffle(numbers);
    ++counts[numbers];
  }

  ASSERT_EQ(counts.size(), 6u);
  for (const auto& [order, count] : counts) {
    EXPECT_NEAR(count, 10000, 600)
        << "order " << order[0] << order[1] << order[2];
  }
}

// Deck shuffles Card objects, the shop and events may shuffle other things.
TEST(RngShuffleTest, WorksForAnyElementType) {
  Rng rng(42);
  std::vector<std::string> names = {"Slash", "Block", "Heal", "Fireball"};
  rng.Shuffle(names);
  std::sort(names.begin(), names.end());

  EXPECT_EQ(names,
            (std::vector<std::string>{"Block", "Fireball", "Heal", "Slash"}));
}
