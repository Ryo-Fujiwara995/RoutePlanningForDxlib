#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"

// �R���X�g���N�^
Enemy::Enemy()
{
    isAlive_ = true;
    // �����ʒu�i��F�L�����N�^�[�T�C�Y�{�I�t�Z�b�g�j
    pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
}

// �f�X�g���N�^
Enemy::~Enemy()
{
}

// �X�V����
void Enemy::Update()
{
    // ���݂̍��W��ۑ�
    int ox = pos_.x, oy = pos_.y;

    // �����_���Ȉړ��ʂ�����
    // GetRand(3) �� 0,1,2 ��Ԃ��̂ŁA -1�`+1 �ɕϊ����邽�߂Ɂu-1�v���܂��B
    Point move = { GetRand(3) - 1, GetRand(3) - 1 };
    pos_.x += move.x;
    pos_.y += move.y;

    // �G�̐V������`
    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };

    // �X�e�[�W�i�ǂ��Q���j�̋�`���擾
    Stage* stage = (Stage*)FindGameObject<Stage>();

    // �e��Q���Ƃ̓����蔻��
    for (auto& obj : stage->GetStageRects())
    {
        if (CheckHit(enemyRect, obj))
        {
            // �����蔻�肪����΁A�܂��e���ʂɓ����蔻�������

            // x �������͌��� x ���W�Ay �͐V�����l
            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            // y �������͌��� y ���W�Ax �͐V�����l
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj))
            {
                // x ���̈ړ��������̏ꍇ�F
                // x ���W�����ɖ߂��Ay ���͊��点��悤�ɒ���
                pos_.x = ox;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                {
                    pos_.y++;  // �G�������ɂ���ꍇ�͉��ւ��炷
                }
                else if (centerEnemy.y < centerObj.y)
                {
                    pos_.y--;  // �G���㑤�ɂ���ꍇ�͏�ւ��炷
                }
            }
            else if (!CheckHit(testRectY, obj))
            {
                // y ���̈ړ��������̏ꍇ�F
                // y ���W�����ɖ߂��Ax ���͊��点��悤�ɒ���
                pos_.y = oy;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.x > centerObj.x)
                {
                    pos_.x++;  // �G���E���ɂ���ꍇ�͉E�ւ��炷
                }
                else if (centerEnemy.x < centerObj.x)
                {
                    pos_.x--;  // �G�������ɂ���ꍇ�͍��ւ��炷
                }
            }
            else
            {
                // ����ł��Փ˂���ꍇ�́A���̍��W�ɖ߂�
                pos_.x = ox;
                pos_.y = oy;
            }

            // ���W�C����A�Փ˔���p�̋�`���X�V
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

// �`�揈��
void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

// ��`���m�̓����蔻��iAABB����j
bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    return (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y);
}
