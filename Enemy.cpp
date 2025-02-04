#include "Enemy.h"
#include "./Stage.h"
#include "Player.h"
#include "globals.h"

Enemy::Enemy()
{
    isAlive_ = true;
    // Enemy�̏����ʒu�������_���Ɍ��߂�
    Stage* stage = FindGameObject<Stage>();
    if (stage){
        pos_ = stage->GetRandomEmptyPosition();
    }
    else{
        // Stage���Ȃ�������A�����ʒu��
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
    targetPos_ = pos_;
}

Enemy::~Enemy()
{
}

// Player���A������ɂ��鎞�ɂ��������Ȃ�
// �����Ȃ��Ƃ��́A�����_���ɓ����悤�ɂ�����...
void Enemy::Update()
{
    Stage* stage = (Stage*)FindGameObject<Stage>();
    Player* player = (Player*)FindGameObject<Player>();
    if (!stage || !player) return;

    // ���݂̍��W��ۑ��i�Փˎ��̖߂��p�j
    int ox = pos_.x, oy = pos_.y;

    // Player�̌��݈ʒu���擾
    Point playerPos = player->GetPosition();

    // Enemy��Player��ǔ����邽�߁APlayer�Ƃ̑��Έʒu�ɉ����Ĉړ�����
    // �������ł̓V���v����x���Ey�����ꂼ��ňړ������Ă��܂�
    int enemySpeed = 1; // �����͑��x�����\
    if (playerPos.x > pos_.x)
        pos_.x += enemySpeed;
    else if (playerPos.x < pos_.x)
        pos_.x -= enemySpeed;

    if (playerPos.y > pos_.y)
        pos_.y += enemySpeed;
    else if (playerPos.y < pos_.y)
        pos_.y -= enemySpeed;

    // �Փ˔���p�̋�`���X�V
    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

    // �ǁi�X�e�[�W�I�u�W�F�N�g�j�Ƃ̓����蔻��ƏՓˉ�������
    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(enemyRect, obj))
        {
            // x���Ey�����ꂼ��P�Ƃňړ��������ꍇ�̋�`��p�ӂ��ă`�F�b�N
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj))
            {
                // x�������̂ݖ߂�
                pos_.x = ox;
                // ����ɕǂɂ߂荞�܂Ȃ��悤��y���������
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                    pos_.y++;
                else if (centerEnemy.y < centerObj.y)
                    pos_.y--;
            }
            else if (!CheckHit(testRectY, obj))
            {
                // y�������̂ݖ߂�
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
                // �����Ƃ��Փ˂��Ă���ꍇ�́A���̈ʒu�ɖ߂�
                pos_.x = ox;
                pos_.y = oy;
            }

            // �C����A�Փ˔���p�̋�`���X�V
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
