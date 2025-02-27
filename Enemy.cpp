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
// 右手法
namespace RightHandRule {
    // 右手法での移動方向
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };

    // 右手法での次の方向
    int nextDir(int dir) {
        return (dir + 3) % 4;
    }

    // 右手法での前方の方向
    int forwardDir(int dir) {
        return dir;
    }

    // 右手法での左方向
    int leftDir(int dir) {
        return (dir + 1) % 4;
    }

    // 右手法での右方向
    int rightDir(int dir) {
        return (dir + 3) % 4;
    }

    // 右手法での移動
    Point move(int x, int y, int dir) {
        return { x + dx[dir], y + dy[dir] };
    }

    // 右手法での移動可能かどうか
    bool canMove(int x, int y, int dir, Stage* stage) {
        Point next = move(x, y, dir);
        return stage->IsPassable(next.x, next.y);
    }

    // 右手法での次の方向を探す
    int findNextDir(int x, int y, int dir, Stage* stage) {
        for (int i = 0; i < 4; i++) {
            int next = rightDir(dir);
            if (canMove(x, y, next, stage))
                return next;
            dir = next;
        }
        return -1;
    }

    // 右手法での経路を求める
    std::vector<Point> findPath(Point start, Point goal, Stage* stage) {
        std::vector<Point> path;
        int x = start.x;
        int y = start.y;
        int dir = 0;
        while (true) {
            path.push_back({ x, y });

            // ゴールに到達したら終了
            if (x == goal.x && y == goal.y)
                break;

            // 右手法に従って移動方向を決定
            int right = rightDir(dir);
            int forward = forwardDir(dir);
            int left = leftDir(dir);

            if (canMove(x, y, right, stage)) {
                dir = right;
            }
            else if (canMove(x, y, forward, stage)) {
                // そのまま前進
            }
            else if (canMove(x, y, left, stage)) {
                dir = left;
            }
            else {
                // どこにも進めない場合、180度回転
                dir = (dir + 2) % 4;
            }

            // 次の位置へ移動
            Point next = move(x, y, dir);
            x = next.x;
            y = next.y;
        }
        return path;
    }
}

// 右手やったから、途中まででいいかな
// 左手法
namespace LeftHandRule {

    // 移動方向（右手法と同じ）
    const int dx[4] = { 1, 0, -1, 0 };
    const int dy[4] = { 0, 1, 0, -1 };

    // 左手法での次の方向
    int nextDir(int dir) {
        return (dir + 1) % 4;
    }

    // 左手法での前方方向
    int forwardDir(int dir) {
        return dir;
    }

    // 左手法での右方向
    int rightDir(int dir) {
        return (dir + 3) % 4;
    }

    // 左手法での左方向
    int leftDir(int dir) {
        return (dir + 1) % 4;
    }

    // 左手法での移動
    Point move(int x, int y, int dir) {
        return { x + dx[dir], y + dy[dir] };
    }

    // 左手法での移動可能判定
    bool canMove(int x, int y, int dir, Stage* stage) {
        Point next = move(x, y, dir);
        return stage->IsPassable(next.x, next.y);
    }

    // 左手法での次の移動方向を決定
    int findNextDir(int x, int y, int dir, Stage* stage) {
        for (int i = 0; i < 4; i++) {
            int next = leftDir(dir); // まず左を優先
            if (canMove(x, y, next, stage))
                return next;
            dir = next; // 次の方向を試す
        }
        return -1;
    }

    // 左手法での経路探索
    std::vector<Point> findPath(Point start, Point goal, Stage* stage) {
        std::vector<Point> path;
        int x = start.x;
        int y = start.y;
        int dir = 0;
        while (true) {
            path.push_back({ x, y });
            if (x == goal.x && y == goal.y)
                break;
            int next = findNextDir(x, y, dir, stage);
            if (next == -1)
                break;
            x += dx[next];
            y += dy[next];
            dir = next;
        }
        return path;
    }
}

// A*
namespace AStar {

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
    chaseMode_(EnemyMode::Random) // 初期はランダム移動
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

    // 追尾範囲　x,y同じ
    int chaseRange = 10;
    // ○○セル分（＝○○*CHA_WIDTH）の距離以内なら追尾モード defalut 5って範囲狭い...??
    const int rangeThresholdSq = (chaseRange * CHA_WIDTH) * (chaseRange * CHA_WIDTH);

    if (distSq <= rangeThresholdSq)
        chaseMode_ = EnemyMode::RightHand;
    else
        chaseMode_ = EnemyMode::Random;

    Point oldPos = pos_;
    int enemySpeed = 1;  // 移動速度

    // bool からswitch に変更
    switch (chaseMode_) {
        // 右手法
    case EnemyMode::RightHand: {
        Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

        int dir = 0;
        while (!(enemyGrid.x == playerGrid.x && enemyGrid.y == playerGrid.y)) {
            int right = RightHandRule::rightDir(dir);
            int forward = RightHandRule::forwardDir(dir);
            int left = RightHandRule::leftDir(dir);

            if (RightHandRule::canMove(enemyGrid.x, enemyGrid.y, right, stage)) {
                dir = right;
            }
            else if (RightHandRule::canMove(enemyGrid.x, enemyGrid.y, forward, stage)) {
                // そのまま前進するので、何もしない
            }
            else if (RightHandRule::canMove(enemyGrid.x, enemyGrid.y, left, stage)) {
                dir = left;
            }
            else {
                dir = (dir + 2) % 4;
            }
            enemyGrid = RightHandRule::move(enemyGrid.x, enemyGrid.y, dir);
            pos_ = { enemyGrid.x * CHA_WIDTH, enemyGrid.y * CHA_HEIGHT };
        }
        break;
    }
    case EnemyMode::LeftHand: {
        Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

        int dir = 0;
        while (!(enemyGrid.x == playerGrid.x && enemyGrid.y == playerGrid.y)) {
            int left = LeftHandRule::leftDir(dir);
            int forward = LeftHandRule::forwardDir(dir);
            int right = LeftHandRule::rightDir(dir);

            if (LeftHandRule::canMove(enemyGrid.x, enemyGrid.y, left, stage)) {
                dir = left;
            }
            else if (LeftHandRule::canMove(enemyGrid.x, enemyGrid.y, forward, stage)) {
                // そのまま前進するので、何もしない
            }
            else if (LeftHandRule::canMove(enemyGrid.x, enemyGrid.y, right, stage)) {
                dir = right;
            }
            else {
                dir = (dir + 2) % 4;
            }

            enemyGrid = LeftHandRule::move(enemyGrid.x, enemyGrid.y, dir);
            pos_ = { enemyGrid.x * CHA_WIDTH, enemyGrid.y * CHA_HEIGHT };
        }
        break;
    }
    case EnemyMode::Random: {
        Point move = { nDir[forward_].x, nDir[forward_].y };
        pos_.x += move.x;
        pos_.y += move.y;

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
    default:
        break;
    }
        //    // 追尾モード: A*でPlayerまでの経路を求める
        //    Point enemyGrid = { pos_.x / CHA_WIDTH, pos_.y / CHA_HEIGHT };
        //    Point playerGrid = { playerPos.x / CHA_WIDTH, playerPos.y / CHA_HEIGHT };

        //    if (path_.empty() || pathIndex_ >= path_.size() || (path_.back().x != playerGrid.x || path_.back().y != playerGrid.y)) {
        //        path_ = AStar::findPath(enemyGrid, playerGrid, stage);
        //        pathIndex_ = 0;
        //    }

        //    if (!path_.empty() && pathIndex_ < path_.size()) {
        //        Point nextNode = path_[pathIndex_];
        //        Point targetPixel = { nextNode.x * CHA_WIDTH, nextNode.y * CHA_HEIGHT };

        //        // X軸方向の補間
        //        if (pos_.x < targetPixel.x) {
        //            pos_.x += enemySpeed;
        //            if (pos_.x > targetPixel.x)
        //                pos_.x = targetPixel.x;
        //        }
        //        else if (pos_.x > targetPixel.x) {
        //            pos_.x -= enemySpeed;
        //            if (pos_.x < targetPixel.x)
        //                pos_.x = targetPixel.x;
        //        }
        //        // Y軸方向の補間
        //        if (pos_.y < targetPixel.y) {
        //            pos_.y += enemySpeed;
        //            if (pos_.y > targetPixel.y)
        //                pos_.y = targetPixel.y;
        //        }
        //        else if (pos_.y > targetPixel.y) {
        //            pos_.y -= enemySpeed;
        //            if (pos_.y < targetPixel.y)
        //                pos_.y = targetPixel.y;
        //        }
        //        // ノードが到達したら、次のノードにいく
        //        if (pos_.x == targetPixel.x && pos_.y == targetPixel.y)
        //            pathIndex_++;
        //    }
        //}


        //else {
        //    // Randomモード
        //    Point move = { nDir[forward_].x, nDir[forward_].y };
        //    pos_.x += move.x;
        //    pos_.y += move.y;

        //    // Stageの壁との判定
        //    Rect eRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        //    for (auto& obj : stage->GetStageRects()) {
        //        if (CheckHit(eRect, obj)) {
        //            pos_ = oldPos;
        //            forward_ = (DIR)(GetRand(4));
        //            break;
        //        }
        //    }
        //    if ((pos_.x % CHA_WIDTH == 0) && (pos_.y % CHA_HEIGHT == 0)) {
        //        forward_ = (DIR)(GetRand(4));
        //    }
        //}
        //// 他にも、壁倒し法、幅優先探索、ダイクストラ法 あるけど

        //Rect enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        //for (auto& obj : stage->GetStageRects()) {
        //    if (CheckHit(enemyRect, obj)) {
        //        Rect testRectX = { oldPos.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        //        Rect testRectY = { pos_.x, oldPos.y, CHA_WIDTH, CHA_HEIGHT };
        //        if (!CheckHit(testRectX, obj))
        //            pos_.x = oldPos.x;
        //        else if (!CheckHit(testRectY, obj))
        //            pos_.y = oldPos.y;
        //        else {
        //            pos_ = oldPos;
        //        }
        //        enemyRect = { pos_.x, pos_.y, CHA_WIDTH, CHA_HEIGHT };
        //    }
        //}
    
}

void Enemy::Draw()
{
    // 敵の基本描画（四角形）
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);

    // 移動モードに応じた三角形の方向表示
    Point tp[4][3] = {
        { {pos_.x + CHA_WIDTH / 2, pos_.y                 }, {pos_.x                , pos_.y + CHA_HEIGHT / 2},{pos_.x + CHA_WIDTH    , pos_.y + CHA_HEIGHT / 2} }, // 上向き
        { {pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT    }, {pos_.x                , pos_.y + CHA_HEIGHT / 2},{pos_.x + CHA_WIDTH    , pos_.y + CHA_HEIGHT / 2} }, // 下向き
        { {pos_.x                , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y                 },{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT    } }, // 左向き
        { {pos_.x + CHA_WIDTH    , pos_.y + CHA_HEIGHT / 2}, {pos_.x + CHA_WIDTH / 2, pos_.y                 },{pos_.x + CHA_WIDTH / 2, pos_.y + CHA_HEIGHT    } }  // 右向き
    };

    int dir = 0;
    int color = GetColor(255, 255, 255); // デフォルトは白

    switch (chaseMode_) {
    case EnemyMode::RightHand:
        dir = RightHandRule::forwardDir(forward_);
        color = GetColor(255, 0, 0); // 赤 (右手法)
        break;

    case EnemyMode::LeftHand:
        dir = LeftHandRule::forwardDir(forward_);
        color = GetColor(0, 255, 0); // 緑 (左手法)
        break;

    case EnemyMode::AStar:
        dir = 0; // A*は方向不要
        color = GetColor(0, 0, 255); // 青 (A*)
        break;

    case EnemyMode::Random:
        dir = forward_;
        color = GetColor(255, 255, 255); // 白 (ランダム)
        break;
    }

    //if (chaseMode_ != EnemyMode::AStar) { // A* は方向指示なし
    //    DrawTriangle(tp[dir][0].x, tp[dir][0].y,
    //        tp[dir][1].x, tp[dir][1].y,
    //        tp[dir][2].x, tp[dir][2].y, color, TRUE);
    //}
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
    if (me.x < other.x + other.w && me.x + me.w > other.x && me.y < other.y + other.h && me.y + me.h > other.y) {
        return true;
    }
    return false;
}
