#include "Enemy.h"
#include "Stage.h"
#include "Player.h"
#include "globals.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

// A* たぶん使うのここだけだからここに書く
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

    // startからgoalまでの経路のPointリストを返す
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

            // 目標に到達していれば終了
            if (current->x == goal.x && current->y == goal.y) {
                pathFound = true;
                goalNode = current;
                break;
            }

            openList.erase(currentIt);
            closedList.push_back(current);

            // 上下右左を調べる
            const int dx[4] = { -1, 1, 0, 0 };
            const int dy[4] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; i++) {
                int nx = current->x + dx[i];
                int ny = current->y + dy[i];

                // グリッドの範囲チェック
                if (nx < 0 || ny < 0 || nx >= STAGE_WIDTH || ny >= STAGE_HEIGHT)
                    continue;
                // 通行可能かチェック
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
                    // openListにあるがより短い経路が見つかった場合は更新
                    if (tentativeG < neighbor->g) {
                        neighbor->g = tentativeG;
                        neighbor->f = neighbor->g + neighbor->h;
                        neighbor->parent = current;
                    }
                }
            }
        }

        if (pathFound && goalNode != nullptr) {
            // goalから経路復元
            Node* current = goalNode;
            while (current) {
                path.push_back({ current->x, current->y });
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
        }

        // メモリ解放
        for (Node* node : openList) {
            delete node;
        }
        for (Node* node : closedList) {
            delete node;
        }

        return path;
    }
}
//////////A*はここまでー//////////

Enemy::Enemy()
{
    //isAlive_ = true;
    
    Stage* stage = FindGameObject<Stage>();
    if (stage) {
        pos_ = stage->GetRandomEmptyPosition();
    }
    else {
        pos_ = { CHA_WIDTH + 64, CHA_HEIGHT + 64 };
    }
    targetPos_ = pos_;


    path_.clear();
    pathIndex_ = 0;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
    Stage* stage = (Stage*)FindGameObject<Stage>();
    Player* player = (Player*)FindGameObject<Player>();
    if (!stage || !player) return;


    int ox = pos_.x, oy = pos_.y;
    int enemySpeed = 1;

    Point playerPos = player->GetPosition();
    Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
    Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

    if (path_.empty() || pathIndex_ >= path_.size() || (path_.back().x != playerGrid.x || path_.back().y != playerGrid.y)) {
        path_ = findPath(enemyGrid, playerGrid, stage);
        pathIndex_ = 0;
    }

    if (!path_.empty() && pathIndex_ < path_.size()) {
        Point nextNode = path_[pathIndex_];
        Point targetPixel = { nextNode.x * CHA_WIDTH, nextNode.y * CHA_HEIGHT };

        if (pos_.x < targetPixel.x) {
            pos_.x += enemySpeed;
            if (pos_.x > targetPixel.x) pos_.x = targetPixel.x;
        }
        else if (pos_.x > targetPixel.x) {
            pos_.x -= enemySpeed;
            if (pos_.x < targetPixel.x) pos_.x = targetPixel.x;
        }
        if (pos_.y < targetPixel.y) {
            pos_.y += enemySpeed;
            if (pos_.y > targetPixel.y) pos_.y = targetPixel.y;
        }
        else if (pos_.y > targetPixel.y) {
            pos_.y -= enemySpeed;
            if (pos_.y < targetPixel.y) pos_.y = targetPixel.y;
        }
        if (pos_.x == targetPixel.x && pos_.y == targetPixel.y) {
            pathIndex_++;
        }
    }

    // 壁WALLとの当たり判定
    Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
    for (auto& obj : stage->GetStageRects()) {
        if (CheckHit(enemyRect, obj)) {

            Rect testRectX = { ox, pos_.y, CHA_WIDTH, CHA_HEIGHT };
            Rect testRectY = { pos_.x, oy, CHA_WIDTH, CHA_HEIGHT };

            if (!CheckHit(testRectX, obj)) {
                pos_.x = ox;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.y > centerObj.y)
                    pos_.y++;
                else if (centerEnemy.y < centerObj.y)
                    pos_.y--;
            }
            else if (!CheckHit(testRectY, obj)) {
                pos_.y = oy;
                Point centerEnemy = Rect{ pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT }.GetCenter();
                Point centerObj = obj.GetCenter();
                if (centerEnemy.x > centerObj.x)
                    pos_.x++;
                else if (centerEnemy.x < centerObj.x)
                    pos_.x--;
            }
            else {
                pos_.x = ox;
                pos_.y = oy;
            }
            enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        }
    }
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT,GetColor(80, 89, 10), TRUE);
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
