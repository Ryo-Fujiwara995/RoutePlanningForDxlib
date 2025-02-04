#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"

Enemy::Enemy()
{
    isAlive_ = true;
    pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
}

Enemy::~Enemy()
{
}
// EnemyをランダムにPlayerに追随させたい
void Enemy::Update()
{
    // 現在の座標を保存
    int ox = pos_.x, oy = pos_.y;

    // ランダムな移動量を決定
    // GetRand(3) は 0,1,2 を返すので、 -1～+1 に変換するために「-1」します。
    Point move = { GetRand(3) - 1, GetRand(3) - 1 };
    pos_.x += move.x;
    pos_.y += move.y;

    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
    Stage* stage = (Stage*)FindGameObject<Stage>();

    for (auto& obj : stage->GetStageRects()){
        if (CheckHit(enemyRect, obj)){
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj)){
                pos_.x = ox;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y){
                    pos_.y++;
                }
                else if (centerEnemy.y < centerObj.y){
                    pos_.y--;
                }
            }
            else if (!CheckHit(testRectY, obj)){
                pos_.y = oy;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.x > centerObj.x){
                    pos_.x++;
                }
                else if (centerEnemy.x < centerObj.x){
                    pos_.x--;
                }
            }
            else{
                pos_.x = ox;
                pos_.y = oy;
            }

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
    return (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y);
}
