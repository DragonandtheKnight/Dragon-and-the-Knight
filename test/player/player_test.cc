#include "player/player.h"

#include <string>

#include "gtest/gtest.h"

// Constructor tests
TEST(PlayerConstructorTest, InitializesPlayerCorrectly) {
  Player player("Knight", 100);

  EXPECT_EQ(player.GetName(), "Knight");
  EXPECT_EQ(player.GetHp(), 100);
  EXPECT_EQ(player.GetMaxHp(), 100);
  EXPECT_EQ(player.GetBlock(), 0);
  EXPECT_EQ(player.GetEnergy(), 0);
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_TRUE(player.IsAlive());
}

TEST(PlayerConstructorTest, InvalidMaxHpBecomesOne) {
  Player player("Knight", 0);

  EXPECT_EQ(player.GetHp(), 1);
  EXPECT_EQ(player.GetMaxHp(), 1);
}

// Damage tests
TEST(PlayerDamageTest, DamageReducesHp) {
  Player player("Knight", 100);

  player.TakeDamage(30);

  EXPECT_EQ(player.GetHp(), 70);
}

TEST(PlayerDamageTest, BlockAbsorbsDamageFirst) {
  Player player("Knight", 100);
  player.AddBlock(20);

  player.TakeDamage(30);

  EXPECT_EQ(player.GetHp(), 90);
  EXPECT_EQ(player.GetBlock(), 0);
}

TEST(PlayerDamageTest, BlockCanAbsorbAllDamage) {
  Player player("Knight", 100);
  player.AddBlock(20);

  player.TakeDamage(10);

  EXPECT_EQ(player.GetHp(), 100);
  EXPECT_EQ(player.GetBlock(), 10);
}

TEST(PlayerDamageTest, HpCannotBeNegative) {
  Player player("Knight", 100);

  player.TakeDamage(200);

  EXPECT_EQ(player.GetHp(), 0);
  EXPECT_FALSE(player.IsAlive());
}

TEST(PlayerDamageTest, NegativeDamageDoesNothing) {
  Player player("Knight", 100);

  player.TakeDamage(-20);

  EXPECT_EQ(player.GetHp(), 100);
}

// Healing tests
TEST(PlayerHealTest, HealingRestoresHp) {
  Player player("Knight", 100);
  player.TakeDamage(40);

  player.Heal(20);

  EXPECT_EQ(player.GetHp(), 80);
}

TEST(PlayerHealTest, HealingCannotExceedMaxHp) {
  Player player("Knight", 100);
  player.TakeDamage(20);

  player.Heal(50);

  EXPECT_EQ(player.GetHp(), 100);
}

TEST(PlayerHealTest, NegativeHealingDoesNothing) {
  Player player("Knight", 100);
  player.TakeDamage(20);

  player.Heal(-10);

  EXPECT_EQ(player.GetHp(), 80);
}

// Block tests
TEST(PlayerBlockTest, AddBlockIncreasesBlock) {
  Player player("Knight", 100);

  player.AddBlock(15);
  player.AddBlock(5);

  EXPECT_EQ(player.GetBlock(), 20);
}

TEST(PlayerBlockTest, NegativeBlockIsIgnored) {
  Player player("Knight", 100);

  player.AddBlock(-10);

  EXPECT_EQ(player.GetBlock(), 0);
}

// Gold tests
TEST(PlayerGoldTest, GainGoldIncreasesGold) {
  Player player("Knight", 100);

  player.GainGold(50);

  EXPECT_EQ(player.GetGold(), 50);
}

TEST(PlayerGoldTest, SpendGoldSucceedsWithEnoughGold) {
  Player player("Knight", 100);
  player.GainGold(50);

  EXPECT_TRUE(player.SpendGold(20));
  EXPECT_EQ(player.GetGold(), 30);
}

TEST(PlayerGoldTest, SpendGoldFailsWithoutEnoughGold) {
  Player player("Knight", 100);
  player.GainGold(10);

  EXPECT_FALSE(player.SpendGold(20));
  EXPECT_EQ(player.GetGold(), 10);
}

TEST(PlayerGoldTest, NegativeGoldOperationsAreIgnored) {
  Player player("Knight", 100);

  player.GainGold(-10);

  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_FALSE(player.SpendGold(-5));
  EXPECT_EQ(player.GetGold(), 0);
}

// Energy tests
TEST(PlayerEnergyTest, ResetEnergySetsEnergyToThree) {
  Player player("Knight", 100);

  player.ResetEnergy();

  EXPECT_EQ(player.GetEnergy(), 3);
}

// Alive status tests
TEST(PlayerAliveTest, PlayerDiesAtZeroHp) {
  Player player("Knight", 100);

  player.TakeDamage(100);

  EXPECT_FALSE(player.IsAlive());
}

TEST(PlayerAliveTest, PlayerCanHealAfterReachingZeroHp) {
  Player player("Knight", 100);
  player.TakeDamage(100);

  player.Heal(10);

  EXPECT_TRUE(player.IsAlive());
  EXPECT_EQ(player.GetHp(), 10);
}
