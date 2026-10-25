#include "enemy/Goblin.h"

Goblin::Goblin(int level)
:Enemy(goblin_type_level(level),goblin_max_hp_level(level),goblin_dfficulty_level(level)) {}

EnemySkill Goblin::calMove(){
    
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
}