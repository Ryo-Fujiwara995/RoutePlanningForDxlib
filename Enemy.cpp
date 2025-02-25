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
    currentDir_ = DIR::RIGHT; // �����̌������E�ɐݒ�
    moveMode_ = 0; // �����̓����_���ړ�

    Stage* stage = (Stage*)FindGameObject<Stage>();
    if (stage) {
        // �ʘH�̍��W���i�[����z��
        std::vector<Point> emptyPositions;

        // �X�e�[�W��S�T�����A�ʘH�̍��W���W�߂�
        for (int y = 0; y < STAGE_HEIGHT; y++) {
            for (int x = 0; x < STAGE_WIDTH; x++) {
                if (stage->GetStageData(x, y) == STAGE_OBJ::EMPTY) {
                    emptyPositions.push_back(Point{ x, y });
                }
            }
        }

        // �ʘH�����������ꍇ�A���̒����烉���_���ɏ����ʒu��ݒ�
        if (!emptyPositions.empty()) {
            int idx = rand() % emptyPositions.size();
            pos_ = {
                emptyPositions[idx].x * CHA_WIDTH,
                emptyPositions[idx].y * CHA_HEIGHT
            };
        }
        else {
            // ������ʘH���Ȃ��ꍇ�̏����ʒu
            pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
        }
    }
    else {
        // stage���擾�ł��Ȃ��ꍇ�̏����ʒu
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
}

// �f�X�g���N�^
Enemy::~Enemy() {}

// �}�b�v���Ŏw��̕����ɐi�߂邩����
bool CanMove(Stage* stage, Point newPos) {
    int gridX = newPos.x / CHA_WIDTH;
    int gridY = newPos.y / CHA_HEIGHT;
    return stage->GetStageData(gridX, gridY) != STAGE_OBJ::WALL;
}

// �����_���ړ�
void Enemy::MoveRandom(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // �J�E���^�[���Z�b�g

    int direction = rand() % 4;
    Point newPos = pos_;

    switch (direction) {
    case 0: newPos.y -= CHA_HEIGHT; break; // ��
    case 1: newPos.y += CHA_HEIGHT; break; // ��
    case 2: newPos.x -= CHA_WIDTH; break;  // ��
    case 3: newPos.x += CHA_WIDTH; break;  // �E
    }

    if (CanMove(stage, newPos)) {
        pos_ = newPos;
    }
}

// �E��@�ňړ�
void Enemy::RightHandRule(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // �J�E���^�[���Z�b�g

    Point moveOffsets[] = {
        {  0, -CHA_HEIGHT }, // ��
        {  0,  CHA_HEIGHT }, // ��
        { -CHA_WIDTH,  0 },  // ��
        {  CHA_WIDTH,  0 }   // �E
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

// ����@�ňړ��i�E��@�̋t�j
void Enemy::LeftHandRule(Stage* stage) {
    if (!stage) return;

    if (moveCounter_ < MOVE_INTERVAL) {
        moveCounter_++;
        return;
    }
    moveCounter_ = 0; // �J�E���^�[���Z�b�g

    Point moveOffsets[] = {
        {  0, -CHA_HEIGHT }, // ��
        {  0,  CHA_HEIGHT }, // ��
        { -CHA_WIDTH,  0 },  // ��
        {  CHA_WIDTH,  0 }   // �E
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

// �X�V�����i1/600 �̊m���ňړ����[�h�����Ԃɐ؂�ւ��j
void Enemy::Update() {
    Stage* stage = (Stage*)FindGameObject<Stage>();

    if (rand() % 600 == 0) {
        moveMode_ = (moveMode_ + 1) % 3; // 0 �� 1 �� 2 �� 0 �̃��[�v
    }

    if (moveMode_ == 0) {
        MoveRandom(stage); // �����_���ړ�
    }
    else if (moveMode_ == 1) {
        RightHandRule(stage); // �E��@
    }
    else {
        LeftHandRule(stage); // ����@
    }
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
