#include "ui/widgets.h"

#include <optional>
#include <vector>

#include "gtest/gtest.h"
#include "ui/screen_text.h"

TEST(RenderKeyHintsTest, ShowsEveryKeyWithItsAction) {
  ftxui::Screen screen =
      Draw(RenderKeyHints({{"e", "End turn"}, {"b", "Deck"}}));

  std::optional<CellPosition> end_turn = FindText(screen, "e End turn");
  std::optional<CellPosition> deck = FindText(screen, "b Deck");
  ASSERT_TRUE(end_turn.has_value());
  ASSERT_TRUE(deck.has_value());
  EXPECT_LT(end_turn->x, deck->x);
}

TEST(RenderKeyHintsTest, NoHintsDrawsNothing) {
  ftxui::Screen screen = Draw(RenderKeyHints({}));

  EXPECT_FALSE(FindText(screen, "e").has_value());
}

TEST(RenderKeyHintsTest, KeysAreBoldAndActionsAreNot) {
  ftxui::Screen screen = Draw(RenderKeyHints({{"e", "End turn"}}));
  std::optional<CellPosition> hint = FindText(screen, "e End turn");
  ASSERT_TRUE(hint.has_value());

  EXPECT_TRUE(screen.CellAt(hint->x, hint->y).bold);
  EXPECT_FALSE(screen.CellAt(hint->x + 2, hint->y).bold);
}
