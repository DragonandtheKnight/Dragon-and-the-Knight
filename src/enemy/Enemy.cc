#include "enemy/Enemy.h"

Enemy::Enemy(EnemyType name,int max_hp,int difficulty){
    name_=name;
    max_hp_=max_hp;
    hp_=max_hp;
    difficulty_=difficulty;
    shield_=0;
    is_alive_=true;
}

void Enemy::set_hp(int hp){
    hp_=hp;
}

int Enemy::get_hp(){
    return hp_;
}

void Enemy::set_max_hp(int max_hp){
    max_hp_=max_hp;
}

int Enemy::get_max_hp(){
    return max_hp_;
}

int Enemy::get_difficulty(){
    return difficulty_;
}

void Enemy::set_shield(int shield){
    shield_=shield;
}

int Enemy::get_shield(){
    return shield_;
}