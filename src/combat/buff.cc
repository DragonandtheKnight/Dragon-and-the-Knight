#include "combat/buff.h"

#include <string>
#include <vector>

void Buff::AddBuff(const Card& card, int count) {
  if (count <= 0) {
    return;
  }
  int i, j, size = buffs_.size();
  for (i = 0; i < size; i++) {
    if (buffs_[i].front().GetName() == card.GetName()) {
      for (j = 0; j < count; j++) {
        buffs_[i].push_back(card);
      }
      return;
    }
  }
  buffs_.push_back(std::vector<Card>());
  for (j = 0; j < count; j++) {
    buffs_[size].push_back(card);
  }
  return;
}

int Buff::GetRemainingRounds(const std::string& name) const {
  int i, size = buffs_.size();
  for (i = 0; i < size; i++) {
    if (buffs_[i].front().GetName() == name) {
      return buffs_[i].size();
    }
  }
  return 0;
}

std::vector<BuffInfo> Buff::GetBuffs() const {
  std::vector<BuffInfo> result;
  int i, size = buffs_.size();
  for (i = 0; i < size; i++) {
    result.push_back({buffs_[i].front().GetName(),
                      buffs_[i].front().GetDescription(),
                      GetRemainingRounds(buffs_[i].front().GetName())});
  }
  return result;
}

void Buff::ApplyBuffs(EffectContext& ctx) {
  int i;
  for (i = 0; i < buffs_.size(); i++) {
    if (buffs_[i].empty()) {
      buffs_.erase(buffs_.begin() + i);
      i--;
      continue;
    }
    Card card = buffs_[i].front();
    card.ApplyEffect(ctx);
    buffs_[i].erase(buffs_[i].begin());
    if (buffs_[i].empty()) {
      buffs_.erase(buffs_.begin() + i);
      i--;
    }
  }
}
