#include "card/deck.h"

#include <cstdint>
#include <cstddef>
#include <utility>

Deck::Deck(uint32_t seed) : rng_(seed) {}

void Deck::AddCard(const Card& card) {
    cards_.push_back(card);
}

const std::vector<Card>& Deck::GetCards() const {
    return cards_;
}

bool Deck::RemoveCard(int index) {

    // Step 1: Check invalid index
    if (index < 0 || static_cast<std::size_t>(index) >= cards_.size()) {
        return false;
    }

    // Step 2: Remove the card
    cards_.erase(cards_.begin() + index);

    // Step 3: Return success
    return true;
}

void Deck::StartBattle() {
    // Step 1: Copy cards into draw pile
    draw_pile_ = cards_;

    // Step 2: Clear hand
    hand_.clear();

    // Step 3: Clear discard pile
    discard_pile_.clear();
}

void Deck::Shuffle() {
    rng_.Shuffle(draw_pile_);
}

void Deck::Draw(int count) {
    for (int i = 0; i < count; ++i) {

        // If the draw pile is empty, recycle discarded cards
        if (draw_pile_.empty()) {
            ReshuffleDiscardPile();
        }

        // No cards available
        if (draw_pile_.empty()) {
            break;
        }

        // Draw the last card 
        hand_.push_back(draw_pile_.back());
        draw_pile_.pop_back();
    }
}


void Deck::ReshuffleDiscardPile() {
    // No discarded cards available
    if (discard_pile_.empty()) {
        return;
    }

    // Move discarded cards into the draw pile
    draw_pile_ = std::move(discard_pile_);

    // Shuffle the new draw pile
    Shuffle();
}


bool Deck::DiscardPlayedCard(int index) {
    // Check if the index is valid
    if (index < 0 ||
        static_cast<std::size_t>(index) >= hand_.size()) {
        return false;
    }

    // Move the played card into the discard pile
    discard_pile_.push_back(std::move(hand_[index]));

    // Remove the played card from the hand
    hand_.erase(hand_.begin() + index);

    return true;
}

void Deck::DiscardHand() {
    // Move all cards from hand to discard pile
    for (Card& card : hand_) {
        discard_pile_.push_back(std::move(card));
    }

    // Clear the hand
    hand_.clear();
}

const std::vector<Card>& Deck::GetHand() const {
    return hand_;
}
