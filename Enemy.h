#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include "./Stage.h"

class Enemy : public GameObject {
    Point pos_;
    bool isAlive_;
    float speed_;
    int moveCounter_;  // フレームカウンター
    static constexpr int MOVE_INTERVAL = 30; // 30フレーム（0.5秒）ごとに移動
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    void MoveRandomly(Stage* stage);
};
