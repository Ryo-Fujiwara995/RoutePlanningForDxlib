#include "Enemy.h"
#include "Stage.h"
#include "globals.h"
#include <cstdlib> // rand() を使用
#include <ctime>   // srand() を初期化

// コンストラクタ
Enemy::Enemy() {
    isAlive_ = true;
    pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 }; // 初期位置
    moveCounter_ = 0; // フレームカウンター初期化
}

// デストラクタ
Enemy::~Enemy() {}

// ランダム移動（一定フレームごと）
void Enemy::MoveRandomly(Stage* stage) {
    if (!stage) return; // ステージが無い場合は処理しない

    // カウンターが一定値に達していなければ移動しない
    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }

    moveCounter_ = 0; // カウンターリセット

    // ランダムな移動方向を決定（0: 上, 1: 下, 2: 左, 3: 右）
    int direction = rand() % 4;
    Point newPos = pos_;

    switch (direction) {
    case 0: newPos.y -= CHA_HEIGHT; break; // 上
    case 1: newPos.y += CHA_HEIGHT; break; // 下
    case 2: newPos.x -= CHA_WIDTH; break;  // 左
    case 3: newPos.x += CHA_WIDTH; break;  // 右
    }

    // マップのグリッド座標を取得
    int gridX = newPos.x / CHA_WIDTH;
    int gridY = newPos.y / CHA_HEIGHT;

    // 壁でない場合のみ移動
    if (stage->GetStageData(gridX, gridY) != STAGE_OBJ::WALL) {
        pos_ = newPos;
    }
}

// 更新処理
void Enemy::Update() {
    Stage* stage = (Stage*)FindGameObject<Stage>();
    MoveRandomly(stage); // 一定間隔でランダム移動
}

// 描画処理
void Enemy::Draw() {
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

// 矩形同士の当たり判定（AABB判定）
bool Enemy::CheckHit(const Rect& me, const Rect& other) {
    return (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y);
}
