#pragma once

#include <string>
#include <vector>

#include "card/card.h"

struct BuffInfo {
  std::string name;
  std::string description;
  int remaining_rounds;
};

class Buff {
 public:
  void AddBuff(const Card& card, int count);
  int GetRemainingRounds(const std::string& name) const;
  std::vector<BuffInfo> GetBuffs() const;

 private:
  std::vector<std::vector<Card>> buffs_;
};
