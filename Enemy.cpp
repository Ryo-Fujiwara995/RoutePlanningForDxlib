#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"

Enemy::Enemy()
    :pos_({ 0,0 }), isAlive_(true)
{
    int rx = GetRand(STAGE_WIDTH * CHA_WIDTH);
    int ry = GetRand(STAGE_HEIGHT * CHA_HEIGHT);
    pos_ = { rx, ry };
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
    Point move = { GetRand(2), GetRand(2) };
    pos_ = { pos_.x + move.x - 1, pos_.y + move.y -1 };
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    return false;
}
