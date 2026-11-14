#include "level/shop.h"

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "card/card.h"
#include "card/card_rarity.h"
#include "card/card_type.h"
#include "gtest/gtest.h"
#include "level/level_type.h"
#include "player/player.h"

namespace {
constexpr uint32_t kSeed = 42;

Card MakeCard(const std::string& name) {
  return Card(name, "Test card", static_cast<CardRarity>(0),
              static_cast<CardType>(0), 1, 5);
}

Shop MakeShop() { return Shop(2, {MakeCard("New Strike")}, {20}); }

void FillDeck(Player& player, int count) {
  for (int i = 0; i < count; ++i) {
    player.GetDeck().AddCard(MakeCard("Original"));
  }
}
}  // namespace

TEST(ShopTest, ReportsShopType) {
  Shop shop = MakeShop();
  EXPECT_EQ(shop.GetType(), LevelType::kShop);
}

TEST(ShopTest, BuySpendsGoldAndAddsCard) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  player.GainGold(30);
  EXPECT_TRUE(shop.Buy(player, 0));
  EXPECT_EQ(player.GetGold(), 10);
  ASSERT_EQ(player.GetDeck().GetCards().size(), 1u);
  EXPECT_EQ(player.GetDeck().GetCards()[0].GetName(), "New Strike");
}

TEST(ShopTest, InvalidIndexChangesNothing) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  player.GainGold(30);
  EXPECT_FALSE(shop.Buy(player, -1));
  EXPECT_FALSE(shop.Buy(player, 99));
  EXPECT_EQ(player.GetGold(), 30);
  EXPECT_TRUE(player.GetDeck().GetCards().empty());
}

TEST(ShopTest, CannotBuyWithoutGold) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  EXPECT_FALSE(shop.Buy(player, 0));
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_TRUE(player.GetDeck().GetCards().empty());
}

TEST(ShopTest, FullDeckRequiresConfirmedExchange) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  FillDeck(player, 8);
  player.GainGold(30);
  EXPECT_FALSE(shop.Buy(player, 0));
  EXPECT_EQ(player.GetGold(), 30);
  EXPECT_EQ(player.GetDeck().GetCards().size(), 8u);
  EXPECT_TRUE(shop.ConfirmExchange(player, 0, 3));
  EXPECT_EQ(player.GetGold(), 10);
  ASSERT_EQ(player.GetDeck().GetCards().size(), 8u);
  EXPECT_EQ(player.GetDeck().GetCards().back().GetName(), "New Strike");
}

TEST(ShopTest, CancelOrInvalidExchangeIsFree) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  FillDeck(player, 8);
  player.GainGold(30);
  EXPECT_FALSE(shop.ConfirmExchange(player, 0, -1));
  EXPECT_FALSE(shop.ConfirmExchange(player, 0, 8));
  EXPECT_FALSE(shop.ConfirmExchange(player, 99, 1));
  EXPECT_EQ(player.GetGold(), 30);
  EXPECT_EQ(player.GetDeck().GetCards().size(), 8u);
}

TEST(ShopTest, SellingCannotBreakEightCardDeck) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  FillDeck(player, 8);
  EXPECT_FALSE(shop.Sell(player, 0));
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_EQ(player.GetDeck().GetCards().size(), 8u);
}

TEST(ShopTest, SellingExtraCardGivesGold) {
  Player player("Knight", 100, kSeed);
  Shop shop = MakeShop();
  FillDeck(player, 9);
  EXPECT_TRUE(shop.Sell(player, 0));
  EXPECT_EQ(player.GetGold(), 5);
  EXPECT_EQ(player.GetDeck().GetCards().size(), 8u);
}
