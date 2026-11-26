#pragma once

#include "card/card.h"

class CardFactory {
 public:
  static Card CreateStrike();
  static Card CreateDefend();
  static Card CreateLifeDrain();
  static Card CreateWhirlwind();
};
