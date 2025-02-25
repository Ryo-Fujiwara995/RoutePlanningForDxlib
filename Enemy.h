#pragma once
#include "./Library/GameObject.h"
#include "Stage.h"
#include "globals.h"

class Enemy : public GameObject {
    Point pos_;
    bool isAlive_;
    float speed_;
    int moveCounter_;  // �t���[���J�E���^�[
    static constexpr int MOVE_INTERVAL = 30; // 30�t���[���i0.5�b�j���ƂɈړ� 60fps = 1s
    DIR currentDir_; // ���݂̌���
    int moveMode_; // 0: �����_���ړ��@1: �E��@�ړ��@2: ����@�ړ�
public:
    Enemy();
    ~Enemy();

    //bool CanMove(Stage* stage, Point newPos);
    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    void MoveRandom(Stage* stage); // �����_���ړ�
    void RightHandRule(Stage* stage);// �E��@�ňړ�
    void LeftHandRule(Stage* stage); // ����@�ňړ�
};
