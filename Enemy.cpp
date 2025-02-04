#include "Enemy.h"
#include "Stage.h"
#include "Player.h"
#include "globals.h"

Enemy::Enemy()
{
    isAlive_ = true;
    // Enemyの初期位置をランダムに決める
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        pos_ = stage->GetRandomEmptyPosition();
    }
    else {
        // Stageがなかったら、初期位置に
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
    targetPos_ = pos_;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
    Stage* stage = (Stage*)FindGameObject<Stage>();
    Player* player = (Player*)FindGameObject<Player>();
    if (!stage || !player)
        return;

    
    int ox = pos_.x, oy = pos_.y;
    int enemySpeed = 1;  // 追尾時の基本速度

   
    Point playerPos = player->GetPosition();

    
    if (moveTimer_ <= 0)
    {
       
        bool aligned = (playerPos.x == pos_.x) || (playerPos.y == pos_.y);
        if (aligned)
        {
            
            if (playerPos.x > pos_.x)
                currentMove_.x = enemySpeed;
            else if (playerPos.x < pos_.x)
                currentMove_.x = -enemySpeed;
            else
                currentMove_.x = 0;

            if (playerPos.y > pos_.y)
                currentMove_.y = enemySpeed;
            else if (playerPos.y < pos_.y)
                currentMove_.y = -enemySpeed;
            else
                currentMove_.y = 0;
        }
        else
        {
            currentMove_.x = GetRand(3) - 1;
            currentMove_.y = GetRand(3) - 1;
        }
        moveTimer_ = 30;
    }
    else
    {
        moveTimer_--;
    }

    pos_.x += currentMove_.x;
    pos_.y += currentMove_.y;

    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(enemyRect, obj))
        {
           
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj))
            {
                
                pos_.x = ox;
               
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                    pos_.y++;
                else if (centerEnemy.y < centerObj.y)
                    pos_.y--;
            }
            else if (!CheckHit(testRectY, obj))
            {
               
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
              
                pos_.x = ox;
                pos_.y = oy;
            }
          
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT,
        GetColor(80, 89, 10), TRUE);
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
