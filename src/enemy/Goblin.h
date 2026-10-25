#pragma once

#include "enemy/Enemy.h"

#include <vector>
#include <string>

class Goblin:public Enemy{
    private:
        EnemyType goblin_type_level(int level);
        int goblin_max_hp_level(int level);
        int goblin_dfficulty_level(int level);
    public:
        Goblin(int level);
        EnemySkill calMove() override;
};