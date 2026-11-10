
#pragma once

#include <cstdint>
#include <vector>

#include "card/card.h"
#include "game/rng.h"

class Deck {
 public:
  explicit Deck(uint32_t seed);

  void AddCard(const Card& card);
  bool RemoveCard(int index);
  const std::vector<Card>& GetCards() const;

  void StartBattle();
  void Shuffle();
  void Draw(int count);

  bool DiscardPlayedCard(int index);
  void DiscardHand();
  const std::vector<Card>& GetHand() const;

 private:
  void ReshuffleDiscardPile();

  std::vector<Card> cards_;
  std::vector<Card> draw_pile_;
  std::vector<Card> hand_;
  std::vector<Card> discard_pile_;

  Rng rng_;
};
