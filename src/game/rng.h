#pragma once

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class Rng {
 public:
  explicit Rng(uint32_t seed);

  // The next value of the mt19937 sequence.
  uint32_t NextU32();

  // A value from min to max, both included. Throws std::invalid_argument if
  // min > max.
  int UniformInt(int min, int max);

  // True percent times in 100. Throws std::invalid_argument unless percent is
  // from 0 to 100.
  bool Chance(int percent);

  // An index from 0 to size - 1. Throws std::invalid_argument if size <= 0.
  int PickIndex(int size);

  // Puts items in a random order, with every order equally likely.
  template <typename T>
  void Shuffle(std::vector<T>& items);

  // The whole engine state as text, so a saved run can continue the same
  // sequence. SetState throws std::invalid_argument if the text isn't a state
  // that GetState produced.
  std::string GetState() const;
  void SetState(const std::string& state);

 private:
  std::mt19937 engine_;
};
