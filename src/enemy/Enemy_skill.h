#pragma once

enum class Enemy_skill {
  Already_dead,

  // slime
  Slime_slap_lv1,
  Slime_heal_lv1,
  Slime_slap_lv2,
  Slime_heal_lv2,
  Slime_slap_lv3,
  Slime_heal_lv3,

  // gobln
  Goblin_hit_lv1,
  Goblin_hit_lv2,
  Goblin_hard_slam_lv2,
  Goblin_hit_lv3,
  Goblin_hard_slam_lv3,
  Goblin_dash_lv3,
  Goblin_dash_attack_lv3,

  // shaman
  Shaman_curse_lv1,
  Shaman_heal_all_lv1,
  Shaman_cures_lv2,
  Shaman_heal_all_lv2,
  Shaman_give_shield_lv2,
  Shaman_cures_lv3,
  Shaman_heal_all_lv3,
  Shaman_give_shield_lv3,
  Shaman_heal_self_lv3,

  Dragon_claw_lv1
};
