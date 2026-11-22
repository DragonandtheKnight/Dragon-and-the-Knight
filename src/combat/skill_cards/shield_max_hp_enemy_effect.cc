#include "combat/skill_cards/shield_max_hp_enemy_effect.h"

#include <memory>

#include "card/effect_context.h"
#include "enemy/enemy.h"
#include "player/player.h"

ShieldMaxHpEnemyEffect::ShieldMaxHpEnemyEffect(int amount) : amount_(amount) {}

void ShieldMaxHpEnemyEffect::Apply(EffectContext& context) const {
  if (context.enemies == nullptr) {
    return;
  }
  int i, max_hp = 0;
  Enemy* enemy = nullptr;
  for (i = 0; i < context.enemies->size(); ++i) {
    if (max_hp < (*context.enemies)[i]->GetMaxHp() &&
        (*context.enemies)[i]->IsAlive()) {
      enemy = (*context.enemies)[i];
      max_hp = enemy->GetMaxHp();
    }
  }
  if (enemy != nullptr) {
    enemy->AddShield(amount_);
  }
}

std::unique_ptr<CardEffect> ShieldMaxHpEnemyEffect::Clone() const {
  return std::make_unique<ShieldMaxHpEnemyEffect>(*this);
}
