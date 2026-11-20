#include "card/card.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "card/effect_context.h"

Card::Card(std::string name, std::string description, CardRarity rarity,
           CardType type, int cost, int value)
    : name_(std::move(name)),
      description_(std::move(description)),
      rarity_(rarity),
      type_(type),
      cost_(cost),
      value_(value) {}

Card::Card(const Card& other)
    : Card(other.name_, other.description_, other.rarity_, other.type_,
           other.cost_, other.value_) {
  effects_.reserve(other.effects_.size());
  for (const auto& effect : other.effects_) {
    AddEffect(effect->Clone());
  }
}

Card& Card::operator=(const Card& other) {
  if (this != &other) {
    // Finish cloning before replacing this card, so a failed clone leaves it
    // unchanged. Move assignment then transfers the completed copy.
    Card copy(other);
    *this = std::move(copy);
  }
  return *this;
}

std::string Card::GetName() const { return name_; }

std::string Card::GetDescription() const { return description_; }

CardRarity Card::GetRarity() const { return rarity_; }

int Card::GetCost() const { return cost_; }

CardType Card::GetType() const { return type_; }

int Card::GetValue() const { return value_; }

void Card::AddEffect(std::unique_ptr<CardEffect> effect) {
  if (!effect) {
    throw std::invalid_argument("Card::AddEffect: effect must not be null");
  }
  effects_.push_back(std::move(effect));
}

void Card::ApplyEffect(EffectContext& context) const {
  for (const auto& effect : effects_) {
    effect->Apply(context);
  }
}

void Card::ApplyEffect(Player& player, Enemy& enemy) const {
  EffectContext context{&player, &enemy};
  ApplyEffect(context);
}
