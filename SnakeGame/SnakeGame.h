#pragma once
#include "raylib.h"
#include <unordered_map>
#include <queue>
#include <cmath>

#include <vector>

#define TILE_SIZE 40
#define SCREEN_W 800
#define SCREEN_H 800

struct Piece
{
	Vector2 pos;
	Color color;

	void Draw()
	{
		DrawRectangleV(pos, { TILE_SIZE, TILE_SIZE }, color);
	}
};

struct Fruit
{
	Vector2 pos;
	bool active = false;
	Color color = RED;

	void NewRandomLocation(Vector2 borderGap)
	{
		int maxCols = (SCREEN_W - borderGap.x) / TILE_SIZE;
		int maxRows = (SCREEN_H - borderGap.y) / TILE_SIZE;

		int col = GetRandomValue(0, maxCols - 1);
		int row = GetRandomValue(0, maxRows - 1);

		pos.x = borderGap.x / 2 + col * TILE_SIZE;
		pos.y = borderGap.y / 2 + row * TILE_SIZE;
	}

	void Draw()
	{
		if (active)
			DrawRectangleV(pos, { TILE_SIZE, TILE_SIZE }, color);
	}
};

struct Cell
{
	int x;
	int y;

	bool operator==(const Cell& other) const
	{
		return x == other.x && y == other.y;
	}

};

struct CellHash
{
	size_t operator()(const Cell& c) const noexcept
	{
		return (std::hash<int>()(c.x) << 1) ^ std::hash<int>()(c.y);
	}
};


class SnakeGame
{
public:
	void Start();
	void Main();
	void Update();
	void Draw();
	void Interaction();
	void AutoMove();
	bool IsTileSafe(Vector2 tile);

	const float tile_size = TILE_SIZE;
	const int screenW = SCREEN_W;
	const int screenH = SCREEN_H;

	Vector2 borderGap = { 40, 40 };

	std::vector<Piece> player;
	Vector2 speed = { tile_size, 0 };

	Fruit fruit;
	Fruit specialFruit;

	bool gameOver = false;
	bool gamePause = false;
	bool canMove = true;
	bool autoPlay = false;
	float autoPlayTimer = 0.0f;

	Color normalColor = GREEN;
	Color autoColor = BLUE;

	int frameCount = 0;
	bool growNextMove = false;

	// a star stuff
	Cell WorldToCell(Vector2 pos);
	Vector2 CellToWorld(Cell c);
	int GetCols();
	int GetRows();
	bool IsCellSafe(const Cell& c);
	bool FindPathAStar(const Cell& start, const Cell& goal, std::vector<Cell>& outPath);

	// A* path storage
	std::vector<Cell> autoPath;

};
