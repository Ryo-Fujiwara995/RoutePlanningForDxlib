#include "Enemy.h"
#include "Stage.h"
#include "Player.h"
#include "globals.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <queue>�@// BreadthFirstSearch
#include <map> // 
#include <stack>
#include "ImGui/imgui.h"
//�@���[�v�͂Ȃ���

namespace {
    Point nDir[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
}
// �E��ƍ��聨���D�恨�_�C�N�X�g����...�̏��ł�����̂ŁA�E�������ǐՑ��x���ꏏ�B���́A�߂��ᑬ��
// --------------------------------------------------
// �{�[�i�X�v�f
// --------------------------------------------------
// �E��@
namespace RightHandRule {
    // �E��@�ł̈ړ�����
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };

    // �E��@�ł̎��̕���
    int nextDir(int dir) {
        return (dir + 3) % 4;
    }

    // �E��@�ł̑O���̕���
    int forwardDir(int dir) {
        return dir;
    }

    // �E��@�ł̍�����
    int leftDir(int dir) {
        return (dir + 1) % 4;
    }

    // �E��@�ł̉E����
    int rightDir(int dir) {
        return (dir + 3) % 4;
    }

    // �E��@�ł̈ړ�
    Point move(int x, int y, int dir) {
        return { x + dx[dir], y + dy[dir] };
    }

    // �E��@�ł̈ړ��\���ǂ���
    bool canMove(int x, int y, int dir, Stage* stage) {
        Point next = move(x, y, dir);
        return stage->IsPassable(next.x, next.y);
    }

    // �E��@�ł̎��̕�����T��
    int findNextDir(int x, int y, int dir, Stage* stage) {
        for (int i = 0; i < 4; i++) {
            int next = rightDir(dir);
            if (canMove(x, y, next, stage))
                return next;
            dir = next;
        }
        return -1;
    }

    // �E��@�ł̌o�H�����߂�
    std::vector<Point> findPath(Point start, Point goal, Stage* stage) {
        std::vector<Point> path;
        int x = start.x;
        int y = start.y;
        int dir = 0;
        while (true) {
            path.push_back({ x, y });

            // �S�[���ɓ��B������I��
            if (x == goal.x && y == goal.y)
                break;

            // �E��@�ɏ]���Ĉړ�����������
            int right = rightDir(dir);
            int forward = forwardDir(dir);
            int left = leftDir(dir);

            if (canMove(x, y, right, stage)) {
                dir = right;
            }
            else if (canMove(x, y, forward, stage)) {
                // ���̂܂ܑO�i
            }
            else if (canMove(x, y, left, stage)) {
                dir = left;
            }
            else {
                // �ǂ��ɂ��i�߂Ȃ��ꍇ�A180�x��]
                dir = (dir + 2) % 4;
            }

            // ���̈ʒu�ֈړ�
            Point next = move(x, y, dir);
            x = next.x;
            y = next.y;
        }
        return path;
    }
}
// ����@
namespace LeftHandRule {

    // �ړ������i�E��@�Ɠ����j
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };

    // ����@�ł̎��̕���
    int nextDir(int dir) {
        return (dir + 1) % 4;
    }

    // ����@�ł̑O������
    int forwardDir(int dir) {
        return dir;
    }

    // ����@�ł̉E����
    int rightDir(int dir) {
        return (dir + 3) % 4;
    }

    // ����@�ł̍�����
    int leftDir(int dir) {
        return (dir + 1) % 4;
    }

    // ����@�ł̈ړ�
    Point move(int x, int y, int dir) {
        return { x + dx[dir], y + dy[dir] };
    }

    // ����@�ł̈ړ��\����
    bool canMove(int x, int y, int dir, Stage* stage) {
        Point next = move(x, y, dir);
        return stage->IsPassable(next.x, next.y);
    }

    // ����@�ł̎��̈ړ�����������
    int findNextDir(int x, int y, int dir, Stage* stage) {
        for (int i = 0; i < 4; i++) {
            int next = leftDir(dir); // �܂�����D��
            if (canMove(x, y, next, stage))
                return next;
            dir = next; // ���̕���������
        }
        return -1;
    }

    // ����@�ł̌o�H�T��
    std::vector<Point> findPath(Point start, Point goal, Stage* stage) {
        std::vector<Point> path;

        // �����ʒu
        int x = start.x;
        int y = start.y;

        // ���������i�E����j
        int dir = 0;

        while (true) {
            path.push_back({ x, y });

            // �S�[���ɓ��B������I��
            if (x == goal.x && y == goal.y)
                break;

            // ����@�ɏ]���Ĉړ�����������
            int left = leftDir(dir);
            int forward = forwardDir(dir);
            int right = rightDir(dir);

            if (canMove(x, y, left, stage)) {
                dir = left;
            }
            else if (canMove(x, y, forward, stage)) {
                // ���̂܂ܑO�i
            }
            else if (canMove(x, y, right, stage)) {
                dir = right;
            }
            else {
                // �ǂ��ɂ��i�߂Ȃ��ꍇ�A180�x��]
                dir = (dir + 2) % 4;
            }

            // ���̈ʒu�ֈړ�
            Point next = move(x, y, dir);
            x = next.x;
            y = next.y;
        }
        return path;
    }

}
// --------------------------------------------------

// �ł����I
// ���D��T��
namespace BreadthFirstSearch {
    std::vector<Point> FindPath(Stage* stage, Point start, Point goal) {
        std::vector<Point> path;
        if (!stage) return path;

        std::queue<Point> queue;
        std::map<Point, Point> cameFrom;  // �o�H�L�^�p
        queue.push(start);
        cameFrom[start] = start;  // �J�n�n�_�̐e�����g�ɐݒ�

        Point directions[] = {
            {0, -1}, {1, 0}, {0, 1}, {-1, 0}  // ��, �E, ��, ��
        };

        while (!queue.empty()) {
            Point current = queue.front();
            queue.pop();

            // �S�[���ɓ��B
            if (current.x == goal.x && current.y == goal.y) {
                break;
            }

            for (const auto& dir : directions) {
                Point next = { current.x + dir.x, current.y + dir.y };

                // �͈͊O�܂��͒ʍs�s�Ȃ�X�L�b�v
                if (!stage->IsPassable(next.x, next.y)) continue;

                // �܂��K��Ă��Ȃ��ꍇ
                if (cameFrom.find(next) == cameFrom.end()) {
                    queue.push(next);
                    cameFrom[next] = current;
                }
            }
        }

        // �o�H���t���ł��ǂ�
        if (cameFrom.find(goal) != cameFrom.end()) {
            Point current = goal;
            while (current.x != start.x || current.y != start.y) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
        }

        return path;  // �ŒZ�o�H��Ԃ�
    }

}

// �ł����I
// �[���D��T��
namespace DepthFirstSearch {
    std::vector<Point> FindPath(Stage* stage, Point start, Point goal) {
        std::vector<Point> path;
        if (!stage) return path;

        std::stack<Point> stack;
        std::map<Point, Point> cameFrom; // �o�H�����p�}�b�v
        stack.push(start);
        cameFrom[start] = start;

        // 4�����̈ړ�
        Point directions[] = {
            {0, -1}, {1, 0}, {0, 1}, {-1, 0}  // ��, �E, ��, ��
        };

        while (!stack.empty()) {
            Point current = stack.top();
            stack.pop();

            // �S�[���ɓ��B������T���I��
            if (current.x == goal.x && current.y == goal.y) {
                break;
            }

            // �אڂ���}�X���`�F�b�N
            for (const auto& dir : directions) {
                Point next = { current.x + dir.x, current.y + dir.y };

                // �ʍs�\���`�F�b�N
                if (!stage->IsPassable(next.x, next.y)) continue;

                // �܂��K��Ă��Ȃ��ꍇ
                if (cameFrom.find(next) == cameFrom.end()) {
                    stack.push(next);
                    cameFrom[next] = current;
                }
            }
        }

        // �o�H���t���ł��ǂ�
        if (cameFrom.find(goal) != cameFrom.end()) {
            Point current = goal;
            while (current.x != start.x || current.y != start.y) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
        }
        return path;  // �o�H��Ԃ�
    }
}

// �ł����I
// �_�C�N�X�g���@
namespace Dijkstra {

    // �D��x�t���L���[�p�̔�r�֐��i���������������ɏ����j
    struct Compare {
        bool operator()(const std::pair<int, Point>& a, const std::pair<int, Point>& b) {
            return a.first > b.first;
        }
    };

    std::vector<Point> FindPath(Stage* stage, Point start, Point goal) {
        std::vector<Point> path;
        if (!stage) return path;

        // �e���W�̍ŒZ�������i�[����}�b�v
        std::map<Point, int> cost;
        std::map<Point, Point> cameFrom;  // �o�H�����p�}�b�v
        std::priority_queue<std::pair<int, Point>, std::vector<std::pair<int, Point>>, Compare> pq;

        // �����ݒ�
        pq.push({ 0, start });
        cost[start] = 0;
        cameFrom[start] = start;

        // 4�����̈ړ�
        Point directions[] = {
            {0, -1}, {1, 0}, {0, 1}, {-1, 0}  // ��, �E, ��, ��
        };

        while (!pq.empty()) {
            auto [currentCost, current] = pq.top();
            pq.pop();

            // �S�[���ɓ��B������T���I��
            if (current.x == goal.x && current.y == goal.y) {
                break;
            }

            // �אڂ���}�X���`�F�b�N
            for (const auto& dir : directions) {
                Point next = { current.x + dir.x, current.y + dir.y };

                // �ʍs�\���`�F�b�N
                if (!stage->IsPassable(next.x, next.y)) continue;

                // �R�X�g���v�Z�i���ׂĂ̈ړ��̓R�X�g1�j
                int newCost = cost[current] + 1;

                // ���Z�����������������ꍇ�A�X�V
                if (cost.find(next) == cost.end() || newCost < cost[next]) {
                    cost[next] = newCost;
                    pq.push({ newCost, next });
                    cameFrom[next] = current;
                }
            }
        }

        // �o�H���t���ł��ǂ�
        if (cameFrom.find(goal) != cameFrom.end()) {
            Point current = goal;
            while (current.x != start.x || current.y != start.y) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
        }

        return path;  // �ŒZ�o�H��Ԃ�
    }
}

// �ł����I�I�@�I�v�V����
// A*
namespace AStar {

    struct Node {
        int x, y;
        int g, h, f;
        Node* parent;
        Node(int _x, int _y, int _g, int _h, Node* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
    };

    // �}���n�b�^������
    int manhattan(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }


    Node* findNode(const std::vector<Node*>& list, int x, int y) {
        for (Node* node : list) {
            if (node->x == x && node->y == y)
                return node;
        }
        return nullptr;
    }

    // start����goal�܂ł̌o�H
    std::vector<Point> findPath(Point start, Point goal, Stage* stage) {
        std::vector<Point> path;
        std::vector<Node*> openList;
        std::vector<Node*> closedList;

        Node* startNode = new Node(start.x, start.y, 0, manhattan(start.x, start.y, goal.x, goal.y), nullptr);
        openList.push_back(startNode);

        bool pathFound = false;
        Node* goalNode = nullptr;

        while (!openList.empty()) {
            auto currentIt = std::min_element(openList.begin(), openList.end(),
                [](Node* a, Node* b) { return a->f < b->f; });
            Node* current = *currentIt;

            if (current->x == goal.x && current->y == goal.y) {
                pathFound = true;
                goalNode = current;
                break;
            }

            openList.erase(currentIt);
            closedList.push_back(current);

            // �㉺���E
            const int dx[4] = { -1, 1, 0, 0 };
            const int dy[4] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; i++) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                if (nx < 0 || ny < 0 || nx >= STAGE_WIDTH || ny >= STAGE_HEIGHT)
                    continue;
                if (!stage->IsPassable(nx, ny))
                    continue;
                if (findNode(closedList, nx, ny))
                    continue;
                int tentativeG = current->g + 1;
                Node* neighbor = findNode(openList, nx, ny);
                if (!neighbor) {
                    int h = manhattan(nx, ny, goal.x, goal.y);
                    neighbor = new Node(nx, ny, tentativeG, h, current);
                    openList.push_back(neighbor);
                }
                else {
                    if (tentativeG < neighbor->g) {
                        neighbor->g = tentativeG;
                        neighbor->f = neighbor->g + neighbor->h;
                        neighbor->parent = current;
                    }
                }
            }
        }

        if (pathFound && goalNode) {
            Node* current = goalNode;
            while (current) {
                path.push_back({ current->x, current->y });
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
        }

        for (Node* node : openList)
            delete node;
        for (Node* node : closedList)
            delete node;

        return path;
    }
}
// �����܂� A*�A���S���Y��

Enemy::Enemy()
    : pos_({ 0, 0 }), isAlive_(true), targetPos_({ 0, 0 }),
    pathIndex_(0), forward_(RIGHT),
    chaseMode_(EnemyMode::Random) // �����̓����_���ړ�
{
    path_.clear(); // �o�H���N���A
    int rx = 0, ry = 0;
    while (rx % 2 == 0 || ry % 2 == 0) {
        rx = GetRand(STAGE_WIDTH - 1);
        ry = GetRand(STAGE_HEIGHT - 1);
    }
    pos_ = { rx * CHA_WIDTH, ry * CHA_HEIGHT };
}

Enemy::~Enemy()
{
}
void Enemy::Imgui()
{
    // ImGui�E�B���h�E�̕\��
    ImGui::Begin("EnemyModeWindow");
    // ���{����g���ꍇ�́Au8������@���Ȃ��ƃG���[
    ImGui::Text(u8"Enemy�̃��[�h��I��ł�������");

    // �{�^���i�����Ă��������Ȃ��j
    if (ImGui::Button("Close Window")) {
        // �����ŃE�B���h�E����鏈����ǉ����邱�Ƃ��\
    }
    ImGui::End();
}
void Enemy::Update()
{
    // ������ւ�ŁAImgui���g������
    Imgui();

    Stage* stage = (Stage*)FindGameObject<Stage>();
    Player* player = (Player*)FindGameObject<Player>();
    if (!stage || !player)
        return;

    Point playerPos = player->GetPosition();
    Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
    Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

    int dx = pos_.x - playerPos.x;
    int dy = pos_.y - playerPos.y;
    int distSq = dx * dx + dy * dy;
    int chaseRange = 50;
    const int rangeThresholdSq = (chaseRange * CHA_WIDTH) * (chaseRange * CHA_WIDTH);

    // ���͈͓��Ƀv���C���[������ꍇ�A�E��@�Œǐ�
    if (distSq <= rangeThresholdSq) {
        //chaseMode_ = EnemyMode::RightHand;
        //chaseMode_ = EnemyMode::LeftHand;
        //chaseMode_ = EnemyMode::Bfs;
        chaseMode_ = EnemyMode::Dfs;
        //chaseMode_ = EnemyMode::Dijkstra;
        //chaseMode_ = EnemyMode::AStar;
    }
    else {
        chaseMode_ = EnemyMode::Random;
    }

    Point oldPos = pos_;
    int enemySpeed = 1;

    switch (chaseMode_) {
    case EnemyMode::Random: {
        // �����_���ړ�
        Point move = { nDir[forward_].x, nDir[forward_].y };
        pos_.x += move.x;
        pos_.y += move.y;

        // �ǂƂ̏Փ˔���
        Rect eRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        for (auto& obj : stage->GetStageRects()) {
            if (CheckHit(eRect, obj)) {
                pos_ = oldPos;
                forward_ = (DIR)(GetRand(4));
                break;
            }
        }
        if ((pos_.x % CHA_WIDTH == 0) && (pos_.y % CHA_HEIGHT == 0)) {
            forward_ = (DIR)(GetRand(4));
        }
        break;
    }
    case EnemyMode::RightHand: {
        // �E��@�ł̌o�H�T��
        if (path_.empty() || pathIndex_ >= path_.size() || (path_.back().x != playerGrid.x || path_.back().y != playerGrid.y)) {
            path_ = RightHandRule::findPath(enemyGrid, playerGrid, stage);
            pathIndex_ = 0;
        }

        // �o�H�ɉ����Ĉړ�
        if (!path_.empty() && pathIndex_ < path_.size()) {
            Point nextNode = path_[pathIndex_];
            Point targetPixel = { nextNode.x * CHA_WIDTH, nextNode.y * CHA_HEIGHT };

            // X�������̈ړ�
            if (pos_.x < targetPixel.x) {
                pos_.x += enemySpeed;
                if (pos_.x > targetPixel.x)
                    pos_.x = targetPixel.x;
            }
            else if (pos_.x > targetPixel.x) {
                pos_.x -= enemySpeed;
                if (pos_.x < targetPixel.x)
                    pos_.x = targetPixel.x;
            }

            // Y�������̈ړ�
            if (pos_.y < targetPixel.y) {
                pos_.y += enemySpeed;
                if (pos_.y > targetPixel.y)
                    pos_.y = targetPixel.y;
            }
            else if (pos_.y > targetPixel.y) {
                pos_.y -= enemySpeed;
                if (pos_.y < targetPixel.y)
                    pos_.y = targetPixel.y;
            }

            // ���̃m�[�h�ɓ��B������A�C���f�b�N�X��i�߂�
            if (pos_.x == targetPixel.x && pos_.y == targetPixel.y) {
                pathIndex_++;
            }
        }
        break;
    }
    case EnemyMode::LeftHand: {
        // ����@�ł̌o�H�T��
        if (path_.empty() || pathIndex_ >= path_.size() || (path_.back().x != playerGrid.x || path_.back().y != playerGrid.y)) {
            path_ = LeftHandRule::findPath(enemyGrid, playerGrid, stage);
            pathIndex_ = 0;
        }

        // �o�H�ɉ����Ĉړ�
        if (!path_.empty() && pathIndex_ < path_.size()) {
            Point nextNode = path_[pathIndex_];
            Point targetPixel = { nextNode.x * CHA_WIDTH, nextNode.y * CHA_HEIGHT };

            // X�������̈ړ�
            if (pos_.x < targetPixel.x) {
                pos_.x += enemySpeed;
                if (pos_.x > targetPixel.x)
                    pos_.x = targetPixel.x;
            }
            else if (pos_.x > targetPixel.x) {
                pos_.x -= enemySpeed;
                if (pos_.x < targetPixel.x)
                    pos_.x = targetPixel.x;
            }

            // Y�������̈ړ�
            if (pos_.y < targetPixel.y) {
                pos_.y += enemySpeed;
                if (pos_.y > targetPixel.y)
                    pos_.y = targetPixel.y;
            }
            else if (pos_.y > targetPixel.y) {
                pos_.y -= enemySpeed;
                if (pos_.y < targetPixel.y)
                    pos_.y = targetPixel.y;
            }

            // ���̃m�[�h�ɓ��B������A�C���f�b�N�X��i�߂�
            if (pos_.x == targetPixel.x && pos_.y == targetPixel.y) {
                pathIndex_++;
            }
        }
        break;
    }
    // �ǔ��������ǂ�����...
    case EnemyMode::Bfs: {
        // �G�ƃv���C���[�̃}�X���W���擾
        Point enemyPos = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerPos = { player->GetPosition().x / CHA_WIDTH, player->GetPosition().y / CHA_HEIGHT };

        // BFS �Ōo�H���擾�i�v���C���[�̓����ɑ����ɑΉ��j
        if (path_.empty()) {
            path_ = BreadthFirstSearch::FindPath(stage, enemyPos, playerPos);
        }

        // �v���C���[�Ƒ��x�𑵂���1�}�X���ǔ�
        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin()); // �擪���폜���Ď��̒n�_��
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }
    // 
    case EnemyMode::Dfs: {
        Point enemyPos = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerPos = { player->GetPosition().x / CHA_WIDTH, player->GetPosition().y / CHA_HEIGHT };

        if (path_.empty()) {
            path_ = DepthFirstSearch::FindPath(stage, enemyPos, playerPos);
        }

        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin());
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }

    case EnemyMode::Dijkstra:{
        // �G�ƃv���C���[�̃}�X���W���擾
        Point enemyPos = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerPos = { player->GetPosition().x / CHA_WIDTH, player->GetPosition().y / CHA_HEIGHT };

        // �_�C�N�X�g���@�Ōo�H���v�Z
        if (path_.empty()) {
            path_ = Dijkstra::FindPath(stage, enemyPos, playerPos);
        }

        // �o�H������ꍇ�A���̃}�X�ֈړ�
        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin()); // �擪���폜���Ď��̒n�_��
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }

	// �ł����I�I
    case EnemyMode::AStar: {
        Point enemyPos = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerPos = { player->GetPosition().x / CHA_WIDTH, player->GetPosition().y / CHA_HEIGHT };

        if (path_.empty()) {
            path_ = AStar::findPath(enemyPos, playerPos, stage);
        }

        if (!path_.empty()) {
            Point next = path_.front();
            path_.erase(path_.begin());
            pos_.x = next.x * CHA_WIDTH;
            pos_.y = next.y * CHA_HEIGHT;
        }
        break;
    }

    default:
        break;
    }
}


void Enemy::Draw()
{
    // �G�̊�{�`��i�l�p�`�j
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    if (me.x < other.x + other.w && me.x + me.w > other.x && me.y < other.y + other.h && me.y + me.h > other.y) {
        return true;
    }
    return false;
}
