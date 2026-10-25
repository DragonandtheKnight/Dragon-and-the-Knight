#include "enemy/Enemy.h"

#include <ctime>
#include <cstdint>

Enemy::Enemy(EnemyType name,int max_hp,int difficulty,int level)
:gen_(static_cast<uint32_t>(std::time(nullptr) ^ reinterpret_cast<uintptr_t>(this))){
    name_=name;
    max_hp_=max_hp;
    hp_=max_hp;
    difficulty_=difficulty;
    level_=level;
    shield_=0;
    is_alive_=true;
}

EnemyType Enemy::get_name(){
    return name_;
}

void Enemy::set_hp(int hp){
    hp_=hp;
}

int Enemy::get_hp(){
    return hp_;
}

int Enemy::get_difficulty(){
    return difficulty_;
}

int Enemy::get_level(){
    return level_;
}

void Enemy::set_shield(int shield){
    shield_=shield;
}

int Enemy::get_shield(){
    return shield_;
}