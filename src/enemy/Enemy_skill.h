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
  Shaman_curse_lv2,
  Shaman_heal_all_lv2,
  Shaman_give_shield_lv2,
  Shaman_curse_lv3,
  Shaman_heal_all_lv3,
  Shaman_give_shield_lv3,
  Shaman_heal_self_lv3,

  // giant
  Giant_slam_lv1,
  Giant_defend_lv1,
  Giant_slam_lv2,
  Giant_defend_lv2,
  Giant_charge_lv2,
  Giant_slam_lv3,
  Giant_defend_lv3,
  Giant_super_charge_lv3,
  Giant_super_charge_hit_lv3,

  // dragon
  Dragon_claw_lv1,
  Dragon_breath_lv1,
  Dragon_claw_lv2,
  Dragon_breath_lv2,
  Dragon_fly_lv2,        // 血量一半以上才可飞，飞起来999防御
  Dragon_drop_slam_lv2,  // 飞起来后才能也必须砸
  Dragon_claw_lv3,
  Dragon_breath_lv3,
  Dragon_fly_lv3,
  Dragon_drop_slam_lv3,
  Dragon_roar_lv3,  // 一局只能用一次，半血以下概率触发，四分之一血以下必然触发
};
