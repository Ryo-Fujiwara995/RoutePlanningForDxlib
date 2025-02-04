#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include <vector>

class Enemy :
    public GameObject
{
    Point pos_;
    //bool isAlive_;
    //float speed_ = 2.0;

    Point targetPos_;
    //int moveTimer_;
    //Point currentMove_;

    // A*�̌o�H
    std::vector<Point> path_;
    int pathIndex_;
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
};

