#pragma once

#include "enemy/EnemyType.h"
#include "enemy/EnemySkill.h"

#include <vector>
#include <string>

class Enemy{
	private:
        EnemyType name_;
		int hp_;
        int max_hp_;
		int difficulty_;
        int shield_;
        bool is_alive_;
        std::vector<EnemySkill> skills_;
	public:
        Enemy(EnemyType name,int maxHp,int difficulty);
        virtual ~Enemy() = default;
		void set_hp(int hp);
        int get_hp();
        void set_max_hp(int max_hp);
        int get_max_hp();
		int get_difficulty();
        void set_shield(int shield);
        int get_shield();
        virtual EnemySkill calMove() = 0;
};