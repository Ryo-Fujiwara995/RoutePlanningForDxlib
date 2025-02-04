#include "Enemy.h"
#include "./Stage.h"
#include "Player.h"
#include "globals.h"

Enemy::Enemy()
{
    isAlive_ = true;
    // Enemyの初期位置をランダムに決める
    Stage* stage = FindGameObject<Stage>();
    if (stage){
        pos_ = stage->GetRandomEmptyPosition();
    }
    else{
        // Stageがなかったら、初期位置に
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
    targetPos_ = pos_;
}

Enemy::~Enemy()
{
}

// Playerが、直線状にいる時にしか動かない
// 動かないときは、ランダムに動くようにしたい...
void Enemy::Update()
{
    Stage* stage = (Stage*)FindGameObject<Stage>();
    Player* player = (Player*)FindGameObject<Player>();
    if (!stage || !player) return;

    // 現在の座標を保存（衝突時の戻し用）
    int ox = pos_.x, oy = pos_.y;

    // Playerの現在位置を取得
    Point playerPos = player->GetPosition();

    // EnemyがPlayerを追尾するため、Playerとの相対位置に応じて移動する
    // ※ここではシンプルにx軸・y軸それぞれで移動させています
    int enemySpeed = 1; // ここは速度調整可能
    if (playerPos.x > pos_.x)
        pos_.x += enemySpeed;
    else if (playerPos.x < pos_.x)
        pos_.x -= enemySpeed;

    if (playerPos.y > pos_.y)
        pos_.y += enemySpeed;
    else if (playerPos.y < pos_.y)
        pos_.y -= enemySpeed;

    // 衝突判定用の矩形を更新
    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

    // 壁（ステージオブジェクト）との当たり判定と衝突解消処理
    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(enemyRect, obj))
        {
            // x軸・y軸それぞれ単独で移動させた場合の矩形を用意してチェック
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj))
            {
                // x軸方向のみ戻す
                pos_.x = ox;
                // さらに壁にめり込まないようにy軸を微調整
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                    pos_.y++;
                else if (centerEnemy.y < centerObj.y)
                    pos_.y--;
            }
            else if (!CheckHit(testRectY, obj))
            {
                // y軸方向のみ戻す
                pos_.y = oy;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.x > centerObj.x)
                    pos_.x++;
                else if (centerEnemy.x < centerObj.x)
                    pos_.x--;
            }
            else
            {
                // 両軸とも衝突している場合は、元の位置に戻す
                pos_.x = ox;
                pos_.y = oy;
            }

            // 修正後、衝突判定用の矩形を更新
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    if (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y)
    {
        return true;
    }
    return false;
}
