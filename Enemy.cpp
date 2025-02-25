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
    currentDir_ = DIR::RIGHT; // 初期の向きを右に設定
    moveMode_ = 0; // 初期はランダム移動

    Stage* stage = (Stage*)FindGameObject<Stage>();
    if (stage) {
        // 通路の座標を格納する配列
        std::vector<Point> emptyPositions;

        // ステージを全探索し、通路の座標を集める
        for (int y = 0; y < STAGE_HEIGHT; y++) {
            for (int x = 0; x < STAGE_WIDTH; x++) {
                if (stage->GetStageData(x, y) == STAGE_OBJ::EMPTY) {
                    emptyPositions.push_back(Point{ x, y });
                }
            }
        }

        // 通路が見つかった場合、その中からランダムに初期位置を設定
        if (!emptyPositions.empty()) {
            int idx = rand() % emptyPositions.size();
            pos_ = {
                emptyPositions[idx].x * CHA_WIDTH,
                emptyPositions[idx].y * CHA_HEIGHT
            };
        }
        else {
            // 万が一通路がない場合の初期位置
            pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
        }
    }
    else {
        // stageが取得できない場合の初期位置
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
}

// デストラクタ
Enemy::~Enemy() {}

// マップ内で指定の方向に進めるか判定
bool CanMove(Stage* stage, Point newPos) {
    int gridX = newPos.x / CHA_WIDTH;
    int gridY = newPos.y / CHA_HEIGHT;
    return stage->GetStageData(gridX, gridY) != STAGE_OBJ::WALL;
}

// ランダム移動
void Enemy::MoveRandom(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // カウンターリセット

    int direction = rand() % 4;
    Point newPos = pos_;

    switch (direction) {
    case 0: newPos.y -= CHA_HEIGHT; break; // 上
    case 1: newPos.y += CHA_HEIGHT; break; // 下
    case 2: newPos.x -= CHA_WIDTH; break;  // 左
    case 3: newPos.x += CHA_WIDTH; break;  // 右
    }

    if (CanMove(stage, newPos)) {
        pos_ = newPos;
    }
}

// 右手法で移動
void Enemy::RightHandRule(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // カウンターリセット

    Point moveOffsets[] = {
        {  0, -CHA_HEIGHT }, // 上
        {  0,  CHA_HEIGHT }, // 下
        { -CHA_WIDTH,  0 },  // 左
        {  CHA_WIDTH,  0 }   // 右
    };

    int rightIndex = (currentDir_ + 1) % 4;
    Point newPos = { pos_.x + moveOffsets[rightIndex].x, pos_.y + moveOffsets[rightIndex].y };

    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        currentDir_ = static_cast<DIR>(rightIndex);
        return;
    }

    newPos = { pos_.x + moveOffsets[currentDir_].x, pos_.y + moveOffsets[currentDir_].y };
    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        return;
    }

    int leftIndex = (currentDir_ + 3) % 4;
    newPos = { pos_.x + moveOffsets[leftIndex].x, pos_.y + moveOffsets[leftIndex].y };
    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        currentDir_ = static_cast<DIR>(leftIndex);
    }
}

// 左手法で移動（右手法の逆）
void Enemy::LeftHandRule(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // カウンターリセット

    Point moveOffsets[] = {
        {  0, -CHA_HEIGHT }, // 上
        {  0,  CHA_HEIGHT }, // 下
        { -CHA_WIDTH,  0 },  // 左
        {  CHA_WIDTH,  0 }   // 右
    };

    int leftIndex = (currentDir_ + 3) % 4;
    Point newPos = { pos_.x + moveOffsets[leftIndex].x, pos_.y + moveOffsets[leftIndex].y };

    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        currentDir_ = static_cast<DIR>(leftIndex);
        return;
    }

    newPos = { pos_.x + moveOffsets[currentDir_].x, pos_.y + moveOffsets[currentDir_].y };
    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        return;
    }

    int rightIndex = (currentDir_ + 1) % 4;
    newPos = { pos_.x + moveOffsets[rightIndex].x, pos_.y + moveOffsets[rightIndex].y };
    if (CanMove(stage, newPos)) {
        pos_ = newPos;
        currentDir_ = static_cast<DIR>(rightIndex);
    }
}

// 更新処理（1/600 の確率で移動モードを順番に切り替え）
void Enemy::Update() {
    Stage* stage = (Stage*)FindGameObject<Stage>();

    if (rand() % 600 == 0) {
        moveMode_ = (moveMode_ + 1) % 3; // 0 → 1 → 2 → 0 のループ
    }

    if (moveMode_ == 0) {
        MoveRandom(stage); // ランダム移動
    }
    else if (moveMode_ == 1) {
        RightHandRule(stage); // 右手法
    }
    else {
        LeftHandRule(stage); // 左手法
    }
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
