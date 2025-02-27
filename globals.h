#pragma once


	struct Point
	{
		int x, y;

		// 等価比較演算子
		bool operator==(const Point& other) const {
			return x == other.x && y == other.y;
		}

		// 不等比較演算子
		bool operator!=(const Point& other) const {
			return !(*this == other);
		}

		// C2676　エラーが出るので、、、
		bool operator<(const Point& other) const {
			return (x < other.x) || (x == other.x && y < other.y);
		}
	};

	struct Pointf
	{
		float x, y;
	};

	struct Rect
	{
		int x, y, w, h;
		Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
		Point GetCenter() { return Point{ x + w / 2, y + h / 2 }; }
	};


	enum class STAGE_OBJ {
		EMPTY,
		WALL,
		GOAL,
		MAX_OBJECT
	};


	enum DIR
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		NONE,
		MAXDIR
	};
	
	const int CHA_WIDTH = 32;
	const int CHA_HEIGHT = 32;

	// 小文字だったので、後で変更
	enum class EnemyMode {
		Random,
		RightHand,
		LeftHand,

		Bfs,// breadth - first search → 幅優先探索
		Dfs,// depth - first search → 深さ優先探索
		
		Dijkstra,// Dijkstra Algorithm
		
		AStar
	};