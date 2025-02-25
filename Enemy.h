#pragma once
#include "./Library/GameObject.h"
#include "Stage.h"
#include "globals.h"

class Enemy : public GameObject {
    Point pos_;
    bool isAlive_;
    float speed_;
    int moveCounter_;  // フレームカウンター
    static constexpr int MOVE_INTERVAL = 30; // 30フレーム（0.5秒）ごとに移動 60fps = 1s
    DIR currentDir_; // 現在の向き
    int moveMode_; // 0: ランダム移動　1: 右手法移動　2: 左手法移動
public:
    Enemy();
    ~Enemy();

    //bool CanMove(Stage* stage, Point newPos);
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    void MoveRandom(Stage* stage); // ランダム移動
    void RightHandRule(Stage* stage);// 右手法で移動
    void LeftHandRule(Stage* stage); // 左手法で移動
};
