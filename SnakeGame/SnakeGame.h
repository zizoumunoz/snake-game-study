#pragma once
#include "raylib.h"
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
        DrawRectangleV(pos, { TILE_SIZE, TILE_SIZE}, color);
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
            DrawRectangleV(pos, { TILE_SIZE, TILE_SIZE}, color);
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
};
