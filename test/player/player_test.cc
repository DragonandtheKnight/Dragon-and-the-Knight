#include "player/player.h"

#include <cstdint>
#include <string>

#include "gtest/gtest.h"

// These tests don't depend on deck order, so any seed works.
constexpr uint32_t kDeckSeed = 42;

// Constructor tests
TEST(PlayerConstructorTest, InitializesPlayerCorrectly) {
  Player player("Knight", 100, kDeckSeed);

  EXPECT_EQ(player.GetName(), "Knight");
  EXPECT_EQ(player.GetHp(), 100);
  EXPECT_EQ(player.GetMaxHp(), 100);
  EXPECT_EQ(player.GetBlock(), 0);
  EXPECT_EQ(player.GetEnergy(), 0);
  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_TRUE(player.IsAlive());
}

TEST(PlayerConstructorTest, InvalidMaxHpBecomesOne) {
  Player player("Knight", 0, kDeckSeed);

  EXPECT_EQ(player.GetHp(), 1);
  EXPECT_EQ(player.GetMaxHp(), 1);
}

TEST(PlayerConstructorTest, NegativeMaxHpBecomesOne) {
  Player player("Knight", -10, kDeckSeed);

  EXPECT_EQ(player.GetHp(), 1);
  EXPECT_EQ(player.GetMaxHp(), 1);
  EXPECT_TRUE(player.IsAlive());
}

// Damage tests
TEST(PlayerDamageTest, DamageReducesHp) {
  Player player("Knight", 100, kDeckSeed);

  player.TakeDamage(30);

  EXPECT_EQ(player.GetHp(), 70);
}

TEST(PlayerDamageTest, BlockAbsorbsDamageFirst) {
  Player player("Knight", 100, kDeckSeed);
  player.AddBlock(20);

  player.TakeDamage(30);

  EXPECT_EQ(player.GetHp(), 90);
  EXPECT_EQ(player.GetBlock(), 0);
}

TEST(PlayerDamageTest, BlockCanAbsorbAllDamage) {
  Player player("Knight", 100, kDeckSeed);
  player.AddBlock(20);

  player.TakeDamage(10);

  EXPECT_EQ(player.GetHp(), 100);
  EXPECT_EQ(player.GetBlock(), 10);
}

TEST(PlayerDamageTest, HpCannotBeNegative) {
  Player player("Knight", 100, kDeckSeed);

  player.TakeDamage(200);

  EXPECT_EQ(player.GetHp(), 0);
  EXPECT_FALSE(player.IsAlive());
}

TEST(PlayerDamageTest, NegativeDamageDoesNothing) {
  Player player("Knight", 100, kDeckSeed);

  player.TakeDamage(-20);

  EXPECT_EQ(player.GetHp(), 100);
}

// Healing tests
TEST(PlayerHealTest, HealingRestoresHp) {
  Player player("Knight", 100, kDeckSeed);
  player.TakeDamage(40);

  player.Heal(20);

  EXPECT_EQ(player.GetHp(), 80);
}

TEST(PlayerHealTest, HealingCannotExceedMaxHp) {
  Player player("Knight", 100, kDeckSeed);
  player.TakeDamage(20);

  player.Heal(50);

  EXPECT_EQ(player.GetHp(), 100);
}

TEST(PlayerHealTest, NegativeHealingDoesNothing) {
  Player player("Knight", 100, kDeckSeed);
  player.TakeDamage(20);

  player.Heal(-10);

  EXPECT_EQ(player.GetHp(), 80);
}

// Block tests
TEST(PlayerBlockTest, AddBlockIncreasesBlock) {
  Player player("Knight", 100, kDeckSeed);

  player.AddBlock(15);
  player.AddBlock(5);

  EXPECT_EQ(player.GetBlock(), 20);
}

TEST(PlayerBlockTest, NegativeBlockIsIgnored) {
  Player player("Knight", 100, kDeckSeed);

  player.AddBlock(-10);

  EXPECT_EQ(player.GetBlock(), 0);
}

// Gold tests
TEST(PlayerGoldTest, GainGoldIncreasesGold) {
  Player player("Knight", 100, kDeckSeed);

  player.GainGold(50);

  EXPECT_EQ(player.GetGold(), 50);
}

TEST(PlayerGoldTest, SpendGoldSucceedsWithEnoughGold) {
  Player player("Knight", 100, kDeckSeed);
  player.GainGold(50);

  EXPECT_TRUE(player.SpendGold(20));
  EXPECT_EQ(player.GetGold(), 30);
}

TEST(PlayerGoldTest, SpendGoldFailsWithoutEnoughGold) {
  Player player("Knight", 100, kDeckSeed);
  player.GainGold(10);

  EXPECT_FALSE(player.SpendGold(20));
  EXPECT_EQ(player.GetGold(), 10);
}

TEST(PlayerGoldTest, NegativeGoldOperationsAreIgnored) {
  Player player("Knight", 100, kDeckSeed);

  player.GainGold(-10);

  EXPECT_EQ(player.GetGold(), 0);
  EXPECT_FALSE(player.SpendGold(-5));
  EXPECT_EQ(player.GetGold(), 0);
}

// Energy tests
TEST(PlayerEnergyTest, EnergyLimitsStartAtTenMaxAndThreePerTurn) {
  Player player("Knight", 100, kDeckSeed);

  EXPECT_EQ(player.GetMaxEnergy(), 10);
  EXPECT_EQ(player.GetEnergyPerTurn(), 3);
}

TEST(PlayerEnergyTest, UnspentEnergyCarriesOver) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  player.GainEnergy(3);

  EXPECT_EQ(player.GetEnergy(), 6);
}

TEST(PlayerEnergyTest, ResetEnergySetsEnergyToZero) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  player.ResetEnergy();

  EXPECT_EQ(player.GetEnergy(), 0);
}

TEST(PlayerEnergyTest, GainEnergyAddsEnergy) {
  Player player("Knight", 100, kDeckSeed);

  player.GainEnergy(1);

  EXPECT_EQ(player.GetEnergy(), 1);
}

TEST(PlayerEnergyTest, GainEnergyCannotExceedMaxEnergy) {
  Player player("Knight", 100, kDeckSeed);

  player.GainEnergy(15);

  EXPECT_EQ(player.GetEnergy(), 10);
}

TEST(PlayerEnergyTest, NonPositiveEnergyGainIsIgnored) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  player.GainEnergy(0);
  player.GainEnergy(-2);

  EXPECT_EQ(player.GetEnergy(), 3);
}

TEST(PlayerEnergyTest, SpendEnergySucceedsWithEnoughEnergy) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  EXPECT_TRUE(player.SpendEnergy(2));
  EXPECT_EQ(player.GetEnergy(), 1);
}

TEST(PlayerEnergyTest, SpendEnergyFailsWithoutEnoughEnergy) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  EXPECT_FALSE(player.SpendEnergy(4));
  EXPECT_EQ(player.GetEnergy(), 3);
}

TEST(PlayerEnergyTest, NegativeEnergySpendFails) {
  Player player("Knight", 100, kDeckSeed);
  player.GainEnergy(3);

  EXPECT_FALSE(player.SpendEnergy(-1));
  EXPECT_EQ(player.GetEnergy(), 3);
}

TEST(PlayerEnergyTest, IncreaseMaxEnergyRaisesMaxEnergy) {
  Player player("Knight", 100, kDeckSeed);

  player.IncreaseMaxEnergy(2);

  EXPECT_EQ(player.GetMaxEnergy(), 12);
  EXPECT_EQ(player.GetEnergy(), 0);
}

TEST(PlayerEnergyTest, IncreaseEnergyPerTurnRaisesEnergyPerTurn) {
  Player player("Knight", 100, kDeckSeed);

  player.IncreaseEnergyPerTurn(1);

  EXPECT_EQ(player.GetEnergyPerTurn(), 4);
  EXPECT_EQ(player.GetEnergy(), 0);
}

TEST(PlayerEnergyTest, NonPositiveEnergyLimitIncreasesAreIgnored) {
  Player player("Knight", 100, kDeckSeed);

  player.IncreaseMaxEnergy(0);
  player.IncreaseMaxEnergy(-2);
  player.IncreaseEnergyPerTurn(0);
  player.IncreaseEnergyPerTurn(-1);

  EXPECT_EQ(player.GetMaxEnergy(), 10);
  EXPECT_EQ(player.GetEnergyPerTurn(), 3);
}

// Alive status tests
TEST(PlayerAliveTest, PlayerDiesAtZeroHp) {
  Player player("Knight", 100, kDeckSeed);

  player.TakeDamage(100);

  EXPECT_FALSE(player.IsAlive());
}

TEST(PlayerAliveTest, DeadPlayerCannotBeHealed) {
  Player player("Knight", 100, kDeckSeed);
  player.TakeDamage(100);

  player.Heal(10);

  EXPECT_FALSE(player.IsAlive());
  EXPECT_EQ(player.GetHp(), 0);
}
