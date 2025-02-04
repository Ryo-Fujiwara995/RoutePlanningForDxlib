#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"

// コンストラクタ
Enemy::Enemy()
{
    isAlive_ = true;
    // 初期位置（例：キャラクターサイズ＋オフセット）
    pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
}

// デストラクタ
Enemy::~Enemy()
{
}

// 更新処理
void Enemy::Update()
{
    // 現在の座標を保存
    int ox = pos_.x, oy = pos_.y;

    // ランダムな移動量を決定
    // GetRand(3) は 0,1,2 を返すので、 -1～+1 に変換するために「-1」します。
    Point move = { GetRand(3) - 1, GetRand(3) - 1 };
    pos_.x += move.x;
    pos_.y += move.y;

    // 敵の新しい矩形
    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

    // ステージ（壁や障害物）の矩形を取得
    Stage* stage = (Stage*)FindGameObject<Stage>();

    // 各障害物との当たり判定
    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(enemyRect, obj))
        {
            // 当たり判定があれば、まず各軸別に当たり判定を試す

            // x 軸方向は元の x 座標、y は新しい値
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            // y 軸方向は元の y 座標、x は新しい値
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj))
            {
                // x 軸の移動が原因の場合：
                // x 座標を元に戻し、y 軸は滑らせるように調整
                pos_.x = ox;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                {
                    pos_.y++;  // 敵が下側にある場合は下へずらす
                }
                else if (centerEnemy.y < centerObj.y)
                {
                    pos_.y--;  // 敵が上側にある場合は上へずらす
                }
            }
            else if (!CheckHit(testRectY, obj))
            {
                // y 軸の移動が原因の場合：
                // y 座標を元に戻し、x 軸は滑らせるように調整
                pos_.y = oy;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.x > centerObj.x)
                {
                    pos_.x++;  // 敵が右側にある場合は右へずらす
                }
                else if (centerEnemy.x < centerObj.x)
                {
                    pos_.x--;  // 敵が左側にある場合は左へずらす
                }
            }
            else
            {
                // それでも衝突する場合は、元の座標に戻す
                pos_.x = ox;
                pos_.y = oy;
            }

            // 座標修正後、衝突判定用の矩形を更新
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

// 描画処理
void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

// 矩形同士の当たり判定（AABB判定）
bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    return (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y);
}
