#include "game/rng.h"

#include <cstdint>
#include <locale>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

Rng::Rng(uint32_t seed) : engine_(seed) {}

uint32_t Rng::NextU32() { return engine_(); }

int Rng::UniformInt(int min, int max) {
  if (min > max) {
    throw std::invalid_argument("Rng::UniformInt: min is greater than max");
  }
  // The algorithm differs between standard libraries, so the same seed gives
  // the same rolls only on the same platform; engine_'s own sequence does not.
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(engine_);
}

bool Rng::Chance(int percent) {
  if (percent < 0 || percent > 100) {
    throw std::invalid_argument("Rng::Chance: percent is outside 0 to 100");
  }
  return UniformInt(0, 99) < percent;
}

int Rng::PickIndex(int size) {
  if (size <= 0) {
    throw std::invalid_argument("Rng::PickIndex: size is not positive");
  }
  return UniformInt(0, size - 1);
}

std::string Rng::GetState() const {
  std::ostringstream out;
  out.imbue(std::locale::classic());
  out << engine_;
  return out.str();
}

void Rng::SetState(const std::string& state) {
  std::istringstream in(state);
  in.imbue(std::locale::classic());
  std::mt19937 parsed;
  in >> parsed;
  if (in.fail()) {
    throw std::invalid_argument("Rng::SetState: text is not an Rng state");
  }

  std::ostringstream round_trip;
  round_trip.imbue(std::locale::classic());
  round_trip << parsed;
  if (round_trip.str() != state) {
    throw std::invalid_argument("Rng::SetState: text is not an Rng state");
  }
  engine_ = parsed;
}
