#include "enemy/Goblin.h"

Goblin::Goblin(int level)
:Enemy(goblin_type_level(level),goblin_max_hp_level(level),goblin_dfficulty_level(level),level) {
    goblin_was_last_move_dash_=false;
}

EnemySkill Goblin::calMove(){
     std::uniform_int_distribution<int> roll(1,100);
     int r=roll(gen_);
    switch (get_level())
    {
    case 1:
        return EnemySkill::GoblinHitLv1;
        break;
    case 2:
        if(r<=70)return EnemySkill::GoblinHitLv2;//70%概率使用技能1
        else return EnemySkill::GoblinHardSlamLv2;
        break;
    case 3:
        if(goblin_was_last_move_dash_==true){//如果上一步是闪避，那么下一步闪避反击
            goblin_was_last_move_dash_=false;
            return EnemySkill::GoblinDashAttackLv3;
        }
        if(r<=30)return EnemySkill::GoblinHitLv3;//70%概率使用技能1
        else if(r<=70)return EnemySkill::GoblinHardSlamLv3;
        else {
            goblin_was_last_move_dash_=true;
            return EnemySkill::GoblinDashLv3;
        }
        break;
    default:
        return EnemySkill::GoblinHitLv1;
        break;
    }
}

EnemyType Goblin::goblin_type_level(int level){
    EnemyType type;
    switch (level)
    {
    case 1:
        type=EnemyType::Goblin;
        break;
    case 2:
        type=EnemyType::Goblin_captain;
        break;
    case 3:
        type=EnemyType::Goblin_king;
        break;
    default:
        type=EnemyType::Goblin;
        break;
    }
    return type;
}

int Goblin::goblin_max_hp_level(int level){
    int temp;
    switch (level)
    {
    case 1:
        temp=30;
        break;
    case 2:
        temp=40;
        break;
    case 3:
        temp=50;
        break;
    default:
        temp=30;
        break;
    }
    return temp;
}

int Goblin::goblin_dfficulty_level(int level){
    int temp;
    switch (level)
    {
    case 1:
        temp=1;
        break;
    case 2:
        temp=2;
        break;
    case 3:
        temp=3;
        break;
    default:
        temp=1;
        break;
    }
    return temp;
}