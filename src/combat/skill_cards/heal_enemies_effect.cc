#include "combat/skill_cards/heal_enemies_effect.h"

#include "card/effect_context.h"
#include "enemy/enemy.h"
#include "player/player.h"

HealEnemiesEffect::HealEnemiesEffect(int percent) : percent_(percent) {}

void HealEnemiesEffect::Apply(EffectContext& context) const {
  if (context.enemies == nullptr) {
    return;
  }
  int i;
  for (i = 0; i < context.enemies->size(); ++i) {
    (*context.enemies)[i]->Heal((*context.enemies)[i]->GetMaxHp() * percent_ /
                                100);
  }
}

std::unique_ptr<CardEffect> HealEnemiesEffect::Clone() const {
  return std::make_unique<HealEnemiesEffect>(*this);
}
