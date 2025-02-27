#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include <vector>

class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    //float speed_ = 2.0;

    Point targetPos_;
    //int moveTimer_;
    //Point currentMove_;

    // A*の経路
    std::vector<Point> path_;
    int pathIndex_;

    DIR forward_;

    //bool chaseMode_;  // true: A*によるPlayer追尾, false: ランダム移動
    //int modeTimer_;
    EnemyMode chaseMode_; // ランダム、右手、左手、A*
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
   
};

