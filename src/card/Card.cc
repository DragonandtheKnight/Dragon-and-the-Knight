#include "card/Card.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "card/effect_context.h"

Card::Card(std::string name, std::string description, CardRarity rarity,
           CardType type, int cost, int value)
    : name(std::move(name)),
      description(std::move(description)),
      rarity(rarity),
      type(type),
      cost(cost),
      value(value) {}

Card::Card(const Card& other)
    : Card(other.name, other.description, other.rarity, other.type, other.cost,
           other.value) {
  effects.reserve(other.effects.size());
  for (const auto& effect : other.effects) {
    addEffect(effect->clone());
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

std::string Card::getName() const {
  return name;
}

std::string Card::getDescription() const {
  return description;
}

CardRarity Card::getRarity() const {
  return rarity;
}

int Card::getCost() const {
  return cost;
}

CardType Card::getType() const {
  return type;
}

int Card::getValue() const {
  return value;
}

void Card::addEffect(std::unique_ptr<CardEffect> effect) {
  if (!effect) {
    throw std::invalid_argument("Card::addEffect: effect must not be null");
  }
  effects.push_back(std::move(effect));
}

void Card::applyEffect(EffectContext& context) const {
  for (const auto& effect : effects) {
    effect->apply(context);
  }
}

void Card::applyEffect(Player& player, Enemy& enemy) const {
  EffectContext context{&player, &enemy};
  applyEffect(context);
}

