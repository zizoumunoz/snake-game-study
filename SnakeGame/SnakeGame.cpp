#include "SnakeGame.h"

void SnakeGame::Start()
{
    Piece head;
    head.color = GREEN;
    head.pos = { borderGap.x / 2, borderGap.y / 2 };
    player.push_back(head);

    Main();
}

void SnakeGame::Main()
{
    InitWindow(screenW, screenH, "Snake Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Update();

        BeginDrawing();
        ClearBackground(DARKGRAY);
        Draw();
        EndDrawing();
    }

    CloseWindow();
}

void SnakeGame::Update()
{
    if (gamePause || gameOver)
        return;

    frameCount++;
    if (autoPlay)
    {
        autoPlayTimer -= GetFrameTime();

        if (autoPlayTimer <= 0.0f)
        {
            autoPlay = false;
            autoPlayTimer = 0.0f;
        }
    }

    Interaction();

    // movement
    if (frameCount % 20 == 0)
    {
        // add tail before moving otherwise snake dies instantly
        if (growNextMove)
        {
            Piece newPiece;
            newPiece.color = DARKGREEN;
            newPiece.pos = player.back().pos; // old tail position
            player.push_back(newPiece);
            growNextMove = false;
        }

        // shift  body
        for (int i = player.size() - 1; i > 0; i--)
            player[i].pos = player[i - 1].pos;

        // move head
        player[0].pos.x += speed.x;
        player[0].pos.y += speed.y;

        canMove = true;
    }


    // selfcollision check
    for (int i = 1; i < player.size(); i++)
    {
        if (player[0].pos.x == player[i].pos.x &&
            player[0].pos.y == player[i].pos.y)
        {
            gameOver = true;
            break;
        }
    }


    // border collision
    if (player[0].pos.x < borderGap.x / 2 ||
        player[0].pos.x > screenW - borderGap.x / 2 - tile_size ||
        player[0].pos.y < borderGap.y / 2 ||
        player[0].pos.y > screenH - borderGap.y / 2 - tile_size)
    {
        gameOver = true;
    }

    // spawn normal fruit
    if (!fruit.active)
    {
        fruit.active = true;
        fruit.color = RED;
        fruit.NewRandomLocation(borderGap);
    }

    // spawn special fruit
    if (!specialFruit.active)
    {
        specialFruit.active = true;
        specialFruit.color = GOLD;
        specialFruit.NewRandomLocation(borderGap);
    }

    // eat normal fruit
    if (player[0].pos.x == fruit.pos.x && player[0].pos.y == fruit.pos.y)
    {
        fruit.active = false;
        growNextMove = true;
        

        

    }

    // Eat special fruit
    if (player[0].pos.x == specialFruit.pos.x && player[0].pos.y == specialFruit.pos.y)
    {
        specialFruit.active = false;

        autoPlay = true;
        autoPlayTimer = 10.0f;   // 10 seconds of autopilot
    }

}

void SnakeGame::Draw()
{
    if (gameOver)
    {
        int txtSize = MeasureText("GAME OVER!", 50);
        DrawText("GAME OVER!", screenW / 2 - txtSize / 2, screenH / 2, 50, RED);
        return;
    }

    // Grid
    for (int i = 0; i < screenW / tile_size + 1; i++)
    {
        int x = borderGap.x / 2 + i * tile_size;
        DrawLine(x, borderGap.y / 2, x, screenH - borderGap.y / 2, BLACK);
    }

    for (int i = 0; i < screenH / tile_size + 1; i++)
    {
        int y = borderGap.y / 2 + i * tile_size;
        DrawLine(borderGap.x / 2, y, screenW - borderGap.x / 2, y, BLACK);
    }

    if (autoPlay)
    {
        int timeLeft = (int)ceilf(autoPlayTimer);
        DrawText(TextFormat("%d", timeLeft), 10, 10, 40, YELLOW);
    }

    // Draw snake
    //for (auto& p : player)
    //    p.Draw();
    for (auto& p : player)
    {
        if (autoPlay)
            DrawRectangleV(p.pos, { tile_size, tile_size }, autoColor);
        else
            DrawRectangleV(p.pos, { tile_size, tile_size }, p.color);
    }


    // Draw fruits
    fruit.Draw();
    specialFruit.Draw();
}

void SnakeGame::Interaction()
{
    if (IsKeyPressed(KEY_P))
        gamePause = !gamePause;

    if (gamePause || gameOver)
        return;

    if (autoPlay)
    {
        AutoMove();
        return;
    }

    if (canMove)
    {
        if (IsKeyPressed(KEY_DOWN) && speed.y == 0)
        {
            speed = { 0, tile_size };
            canMove = false;
        }
        if (IsKeyPressed(KEY_UP) && speed.y == 0)
        {
            speed = { 0, -tile_size };
            canMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && speed.x == 0)
        {
            speed = { -tile_size, 0 };
            canMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && speed.x == 0)
        {
            speed = { tile_size, 0 };
            canMove = false;
        }
    }
}


// replace with an alg later so that it can go around walls
void SnakeGame::AutoMove()
{
    Vector2 head = player[0].pos;

    // Candidate moves
    Vector2 right = { head.x + tile_size, head.y };
    Vector2 left = { head.x - tile_size, head.y };
    Vector2 down = { head.x, head.y + tile_size };
    Vector2 up = { head.x, head.y - tile_size };

    // -----------------------------
    // 1. GREEDY TOWARD FRUIT
    //    (allow continuing straight, block reversing)
    // -----------------------------

    // Fruit to the right
    if (fruit.pos.x > head.x && speed.x >= 0 && IsTileSafe(right))
    {
        speed = { tile_size, 0 };
        canMove = false;
        return;
    }

    // Fruit to the left
    if (fruit.pos.x < head.x && speed.x <= 0 && IsTileSafe(left))
    {
        speed = { -tile_size, 0 };
        canMove = false;
        return;
    }

    // Fruit below
    if (fruit.pos.y > head.y && speed.y >= 0 && IsTileSafe(down))
    {
        speed = { 0, tile_size };
        canMove = false;
        return;
    }

    // Fruit above
    if (fruit.pos.y < head.y && speed.y <= 0 && IsTileSafe(up))
    {
        speed = { 0, -tile_size };
        canMove = false;
        return;
    }

    // -----------------------------
    // 2. FALLBACK: ANY SAFE MOVE
    //    (block only reverse direction)
    // -----------------------------

    // Right (allowed unless moving left)
    if (IsTileSafe(right) && speed.x >= 0)
    {
        speed = { tile_size, 0 };
        canMove = false;
        return;
    }

    // Left (allowed unless moving right)
    if (IsTileSafe(left) && speed.x <= 0)
    {
        speed = { -tile_size, 0 };
        canMove = false;
        return;
    }

    // Down (allowed unless moving up)
    if (IsTileSafe(down) && speed.y >= 0)
    {
        speed = { 0, tile_size };
        canMove = false;
        return;
    }

    // Up (allowed unless moving down)
    if (IsTileSafe(up) && speed.y <= 0)
    {
        speed = { 0, -tile_size };
        canMove = false;
        return;
    }

    // If absolutely no safe move exists, do nothing.
}

bool SnakeGame::IsTileSafe(Vector2 tile)
{
    // Border
    if (tile.x < borderGap.x / 2 ||
        tile.x > screenW - borderGap.x / 2 - tile_size ||
        tile.y < borderGap.y / 2 ||
        tile.y > screenH - borderGap.y / 2 - tile_size)
        return false;

    // Body collision (skip head at index 0)
    for (int i = 1; i < player.size(); i++)
        if (player[i].pos.x == tile.x && player[i].pos.y == tile.y)
            return false;

    return true;
}


