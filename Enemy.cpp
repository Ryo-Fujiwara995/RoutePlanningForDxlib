#include "Enemy.h"
#include "Stage.h"
#include "Player.h"
#include "globals.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>


namespace {
    Point nDir[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
}


// A*
namespace {

    struct Node {
        int x, y;
        int g, h, f;
        Node* parent;
        Node(int _x, int _y, int _g, int _h, Node* _parent)
            : x(_x), y(_y), g(_g), h(_h), f(_g + _h), parent(_parent) {}
    };

    // マンハッタン距離
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

    // startからgoalまでの経路
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

            // 上下左右
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
// ここまで A*アルゴリズム

Enemy::Enemy()
    : pos_({ 0, 0 }), isAlive_(true), targetPos_({ 0, 0 }),
    pathIndex_(0), forward_(RIGHT),
    chaseMode_(false) // 初期はランダム
{
    
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

void Enemy::Update()
{
    Stage* stage = (Stage*)FindGameObject<Stage>();
    if (!stage)
        return;

    Player* player = (Player*)FindGameObject<Player>();
    if (!player)
        return;


    Point playerPos = player->GetPosition();
    int dx = pos_.x - playerPos.x;
    int dy = pos_.y - playerPos.y;
    int distSq = dx * dx + dy * dy;

    // ○○セル分（＝○○*CHA_WIDTH）の距離以内なら追尾モード 5って範囲狭い...??
    const int rangeThresholdSq = (300 * CHA_WIDTH) * (300 * CHA_WIDTH);
    if (distSq <= rangeThresholdSq)
        chaseMode_ = true;
    else
        chaseMode_ = false;
    Point oldPos = pos_;
    int enemySpeed = 1;  // 移動速度

    if (chaseMode_) { // chaseMode_ 2つしかないからboolでいいか
        // 追尾モード: A*でPlayerまでの経路を求め
        Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

        if (path_.empty() || pathIndex_ >= path_.size() || (path_.back().x != playerGrid.x || path_.back().y != playerGrid.y)) {
            path_ = findPath(enemyGrid, playerGrid, stage);
            pathIndex_ = 0;
        }

        if (!path_.empty() && pathIndex_ < path_.size()) {
            Point nextNode = path_[pathIndex_];
            Point targetPixel = { nextNode.x * CHA_WIDTH, nextNode.y * CHA_HEIGHT };

            // X軸方向の補間
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
            // Y軸方向の補間
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
            // ノードが到達したら、は次のノードにいく
            if (pos_.x == targetPixel.x && pos_.y == targetPixel.y)
                pathIndex_++;
        }
    }


    else {
        // Randomモード
        Point move = { nDir[forward_].x, nDir[forward_].y };
        pos_.x += move.x;
        pos_.y += move.y;

        // Stageの壁との判定
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
    }
    // 他にも、壁倒し法、幅優先探索、ダイクストラ法 あるけど

    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
    for (auto& obj : stage->GetStageRects()) {
        if (CheckHit(enemyRect, obj)) {
            Rect testRectX = { oldPos.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oldPos.y, CHA_WIDTH, CHA_HEIGHT };
            if (!CheckHit(testRectX, obj))
                pos_.x = oldPos.x;
            else if (!CheckHit(testRectY, obj))
                pos_.y = oldPos.y;
            else {
                pos_ = oldPos;
            }
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT,GetColor(80, 89, 10), TRUE);

    // Randomモードの場合は、向いている方向を示す三角形を描画
    if (!chaseMode_) {
        Point tp[4][3] = {
            { {pos_.x + CHA_WIDTH / 2, pos_.y},
              {pos_.x, pos_.y + CHA_HEIGHT / 2},
              {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2} },
            { {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT},
              {pos_.x, pos_.y + CHA_HEIGHT / 2},
              {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2} },
            { {pos_.x, pos_.y + CHA_HEIGHT / 2},
              {pos_.x + CHA_WIDTH / 2, pos_.y},
              {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT} },
            { {pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT / 2},
              {pos_.x + CHA_WIDTH / 2, pos_.y},
              {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT} }
        };
        DrawTriangle(   tp[forward_][0].x, tp[forward_][0].y,
                        tp[forward_][1].x, tp[forward_][1].y,
                        tp[forward_][2].x, tp[forward_][2].y,GetColor(255, 255, 255), TRUE);
    }
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    if (me.x < other.x + other.w && me.x + me.w > other.x && me.y < other.y + other.h && me.y + me.h > other.y) {
        return true;
    }
    return false;
}
