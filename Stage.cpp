#include "Stage.h"
#include "./globals.h"

Stage::Stage()
{
	stageData = vector(STAGE_HEIGHT, vector<STAGE_OBJ>(STAGE_WIDTH, STAGE_OBJ::EMPTY));
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (y == 0 || y == STAGE_HEIGHT - 1 || x == 0 || x == STAGE_WIDTH - 1)
			{
				stageData[y][x] = STAGE_OBJ::WALL;
			}
			else
			{
				if (x % 2 == 0 && y % 2 == 0)
					stageData[y][x] = STAGE_OBJ::WALL;
				else
					stageData[y][x] = STAGE_OBJ::EMPTY;
			}
		}
	}
	setStageRects();
}

Stage::~Stage()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			switch (stageData[y][x])
			{
			case STAGE_OBJ::EMPTY:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(102, 205, 170), TRUE);
				break;
			case STAGE_OBJ::WALL:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(119, 136, 153), TRUE);
				break;
			case STAGE_OBJ::GOAL:
			
				break;
			default:
				break;
			}
		}
	}
}

void Stage::setStageRects()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x] == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT,  CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}

}

Point Stage::GetRandomEmptyPosition()
{
	vector<Point> emptyPositions;
	for (int y = 0; y < STAGE_HEIGHT; y++){
		for (int x = 0; x < STAGE_WIDTH; x++){
			if (stageData[y][x] == STAGE_OBJ::EMPTY){
				emptyPositions.push_back({ x * CHA_WIDTH, y * CHA_HEIGHT });
			}
		}
	}
	if (!emptyPositions.empty()){
		int index = GetRand(emptyPositions.size());
		return emptyPositions[index];
	}
	return { CHA_WIDTH, CHA_HEIGHT };
}

bool Stage::IsPassable(int gridX, int gridY) {
	if (gridX < 0 || gridX >= STAGE_WIDTH || gridY < 0 || gridY >= STAGE_HEIGHT)
		return false;
	return stageData[gridY][gridX] == STAGE_OBJ::EMPTY;
}
