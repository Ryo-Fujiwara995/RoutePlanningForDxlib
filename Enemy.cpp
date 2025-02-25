#include "Enemy.h"
#include "Stage.h"
#include "globals.h"
#include <cstdlib> // rand() ���g�p
#include <ctime>   // srand() ��������

// �R���X�g���N�^
Enemy::Enemy() {
    isAlive_ = true;
    pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 }; // �����ʒu
    moveCounter_ = 0; // �t���[���J�E���^�[������
}

// �f�X�g���N�^
Enemy::~Enemy() {}

// �����_���ړ��i���t���[�����Ɓj
void Enemy::MoveRandomly(Stage* stage) {
    if (!stage) return; // �X�e�[�W�������ꍇ�͏������Ȃ�

    // �J�E���^�[�����l�ɒB���Ă��Ȃ���Έړ����Ȃ�
    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }

    moveCounter_ = 0; // �J�E���^�[���Z�b�g

    // �����_���Ȉړ�����������i0: ��, 1: ��, 2: ��, 3: �E�j
    int direction = rand() % 4;
    Point newPos = pos_;

    switch (direction) {
    case 0: newPos.y -= CHA_HEIGHT; break; // ��
    case 1: newPos.y += CHA_HEIGHT; break; // ��
    case 2: newPos.x -= CHA_WIDTH; break;  // ��
    case 3: newPos.x += CHA_WIDTH; break;  // �E
    }

    // �}�b�v�̃O���b�h���W���擾
    int gridX = newPos.x / CHA_WIDTH;
    int gridY = newPos.y / CHA_HEIGHT;

    // �ǂłȂ��ꍇ�݈̂ړ�
    if (stage->GetStageData(gridX, gridY) != STAGE_OBJ::WALL) {
        pos_ = newPos;
    }
}

// �X�V����
void Enemy::Update() {
    Stage* stage = (Stage*)FindGameObject<Stage>();
    MoveRandomly(stage); // ���Ԋu�Ń����_���ړ�
}

// �`�揈��
void Enemy::Draw() {
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

// ��`���m�̓����蔻��iAABB����j
bool Enemy::CheckHit(const Rect& me, const Rect& other) {
    return (me.x < other.x + other.w &&
        me.x + me.w > other.x &&
        me.y < other.y + other.h &&
        me.y + me.h > other.y);
}
