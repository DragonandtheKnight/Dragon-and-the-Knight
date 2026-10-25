#pragma once

#include "enemy/EnemyType.h"
#include "enemy/EnemySkill.h"

#include <vector>
#include <string>
#include <random>

class Enemy{
	private:
        EnemyType name_;
		int hp_;
        int max_hp_;
		int difficulty_;
        int shield_;
        int level_;
        bool is_alive_;
    protected:
        std::mt19937 gen_;
        std::vector<EnemySkill> skills_;
	public:
        Enemy(EnemyType name,int maxHp,int difficulty,int level);
        virtual ~Enemy() = default;
        EnemyType get_name();
		void set_hp(int hp);
        int get_hp();
		int get_difficulty();
        int get_level();
        void set_shield(int shield);
        int get_shield();
        virtual EnemySkill calMove() = 0;
};