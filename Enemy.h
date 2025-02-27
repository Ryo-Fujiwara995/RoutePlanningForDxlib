#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include <vector>

class Enemy :
    public GameObject
{
private:


    Point pos_;
    bool isAlive_;

    Point targetPos_;

    // A*�̌o�H
    std::vector<Point> path_;
    int pathIndex_;

    DIR forward_;

    //bool chaseMode_;  // true: A*�ɂ��Player�ǔ�, false: �����_���ړ�
    //int modeTimer_;
    EnemyMode chaseMode_; // �����_���A�E��A����AA*
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
   
};