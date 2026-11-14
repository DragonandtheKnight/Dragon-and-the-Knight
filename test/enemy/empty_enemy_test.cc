// Tests for EmptyEnemy, the placeholder that fills a battle slot with no monster.
//
// A battle always has four slots (kMaxEnemySlots). A slot that holds no monster
// is not a nullptr: it holds an EmptyEnemy, so damage, heal and shield code can
// sweep the whole array without a null check on every call site. The placeholder
// is born dead - it is built with a zero cap, which is what Enemy uses to decide
// is_alive_ - and it inherits the "a corpse cannot change state" guards from
// Enemy instead of overriding them.
//
// What the rest of the game is allowed to rely on:
//   IsEmpty() is true and IsAlive() is false from construction;
//   CalculateMove() reports kAlreadyDead, so the slot never shows an intent;
//   TakeDamage / Heal / AddShield leave hp and shield at zero;
//   the type it carries is EnemyType::kEmpty, which is what IsEmpty() reads.
#include "enemy/empty_enemy.h"

#include <cstdint>

#include "enemy/enemy_skill.h"
#include "enemy/enemy_type.h"
#include "gtest/gtest.h"

namespace {

constexpr std::uint32_t kSeed = 7;

}  // namespace

TEST(EmptyEnemyTest, IsEmptyAndNotAliveFromConstruction) {
  EmptyEnemy slot(kSeed);
  EXPECT_TRUE(slot.IsEmpty());
  EXPECT_FALSE(slot.IsAlive());
}

TEST(EmptyEnemyTest, CarriesTheEmptyType) {
  EmptyEnemy slot(kSeed);
  EXPECT_EQ(slot.GetName(), EnemyType::kEmpty);
}

// Zero cap on purpose: nothing should ever draw a health bar for this slot, and
// a zero cap is what makes the placeholder start dead.
TEST(EmptyEnemyTest, HasNoHealth) {
  EmptyEnemy slot(kSeed);
  EXPECT_EQ(slot.GetHp(), 0);
  EXPECT_EQ(slot.GetMaxHp(), 0);
  EXPECT_EQ(slot.GetShield(), 0);
}

// A slot with no monster has no intent to show, so the battle code can refresh
// next_acts_ for every slot without asking which ones are real.
TEST(EmptyEnemyTest, NeverReportsAnIntent) {
  EmptyEnemy slot(kSeed);
  EXPECT_EQ(slot.CalculateMove(), EnemySkill::kAlreadyDead);
}

// Area damage sweeps every slot; the placeholder absorbs the call and keeps the
// battle code free of null checks.
TEST(EmptyEnemyTest, IgnoresDamage) {
  EmptyEnemy slot(kSeed);
  slot.TakeDamage(5);
  EXPECT_EQ(slot.GetHp(), 0);
  EXPECT_FALSE(slot.IsAlive());
}

TEST(EmptyEnemyTest, IgnoresHealing) {
  EmptyEnemy slot(kSeed);
  slot.Heal(10);
  EXPECT_EQ(slot.GetHp(), 0);
}

// A shaman that heals or shields the whole enemy line must not leave a shield
// bar sitting on an empty slot.
TEST(EmptyEnemyTest, IgnoresShield) {
  EmptyEnemy slot(kSeed);
  slot.AddShield(10);
  EXPECT_EQ(slot.GetShield(), 0);
}

// Clearing a shield on the placeholder is harmless and must not wake it up.
TEST(EmptyEnemyTest, ClearShieldKeepsItDead) {
  EmptyEnemy slot(kSeed);
  slot.ClearShield();
  EXPECT_EQ(slot.GetShield(), 0);
  EXPECT_FALSE(slot.IsAlive());
}
