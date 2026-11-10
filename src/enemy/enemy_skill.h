#pragma once

enum class EnemySkill {
  kAlreadyDead,

  // slime
  kSlimeSlapLv1,
  kSlimeHealLv1,
  kSlimeSlapLv2,
  kSlimeHealLv2,
  kSlimeSlapLv3,
  kSlimeHealLv3,

  // gobln
  kGoblinHitLv1,
  kGoblinHitLv2,
  kGoblinHardSlamLv2,
  kGoblinHitLv3,
  kGoblinHardSlamLv3,
  kGoblinDashLv3,
  kGoblinDashAttackLv3,

  // shaman
  kShamanCurseLv1,
  kShamanHealAllLv1,
  kShamanCurseLv2,
  kShamanHealAllLv2,
  kShamanGiveShieldLv2,
  kShamanCurseLv3,
  kShamanHealAllLv3,
  kShamanGiveShieldLv3,
  kShamanHealSelfLv3,

  // giant
  kGiantSlamLv1,
  kGiantDefendLv1,
  kGiantSlamLv2,
  kGiantDefendLv2,
  kGiantChargeLv2,
  kGiantSlamLv3,
  kGiantDefendLv3,
  kGiantSuperChargeLv3,
  kGiantSuperChargeHitLv3,

  // dragon
  kDragonClawLv1,
  kDragonBreathLv1,
  kDragonClawLv2,
  kDragonBreathLv2,
  kDragonFlyLv2,       // 血量一半以上才可飞，飞起来999防御
  kDragonDropSlamLv2,  // 飞起来后才能也必须砸
  kDragonClawLv3,
  kDragonBreathLv3,
  kDragonFlyLv3,
  kDragonDropSlamLv3,
  kDragonRoarLv3,  // 一局只能用一次，半血以下概率触发，四分之一血以下必然触发
};
