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
        autoPath.clear(); //  for next fruit
    }


    // Eat special fruit
    if (player[0].pos.x == specialFruit.pos.x && player[0].pos.y == specialFruit.pos.y)
    {
        specialFruit.active = false;

        autoPlay = true;
        autoPlayTimer = 10.0f;   // 10 seconds of autopilot
        autoPath.clear();

    }

    if (player[0].pos.x == fruit.pos.x && player[0].pos.y == fruit.pos.y)
    {
        fruit.active = false;
        growNextMove = true;
        autoPath.clear();
        debugPath.clear();
    }

    if (player[0].pos.x == specialFruit.pos.x && player[0].pos.y == specialFruit.pos.y)
    {
        specialFruit.active = false;
        autoPlay = true;
        autoPlayTimer = 10.0f;
        autoPath.clear();
        debugPath.clear();
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
    // a star drawing
    if (autoPlay)
    {
        for (auto& c : debugPath)
        {
            Vector2 p = CellToWorld(c);
            DrawRectangle(p.x, p.y, tile_size, tile_size, Fade(YELLOW, 0.35f));
        }
    }

    DrawText(TextFormat("%d", (int)ceilf(autoPlayTimer)), 10, 10, 40, YELLOW);


    // Draw snake
    for (auto& p : player)
    {
        if (autoPlay)
            DrawRectangleV(p.pos, { tile_size, tile_size }, autoColor);
        else
            DrawRectangleV(p.pos, { tile_size, tile_size }, p.color);
    }

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


    Cell headCell = WorldToCell(player[0].pos);
    Cell fruitCell = WorldToCell(fruit.pos);

    // If the next expected cell is NOT the next in the path → recompute
    if (!autoPath.empty())
    {
        Cell expected = autoPath.front();
        Cell actual = headCell;

        // If head is NOT where we expect it to be relative to the path
        if (!(expected.x == actual.x || expected.y == actual.y))
        {
            autoPath.clear();
            debugPath.clear();
        }
    }


    // Compute path if needed
    if (autoPath.empty())
    {
        // Only recompute if we are NOT already on the goal
        if (!(headCell.x == fruitCell.x && headCell.y == fruitCell.y))
        {
            std::vector<Cell> path;
            if (FindPathAStar(headCell, fruitCell, path))
            {
                autoPath = path;
                debugPath = path;
            }
            else
            {
                return;
            }
        }
    }


    if (autoPath.empty())
        return;

    // Next step
    Cell next = autoPath.front();
    autoPath.erase(autoPath.begin());

    // Validate adjacency (prevent diagonal)
    int dx = next.x - headCell.x;
    int dy = next.y - headCell.y;

    bool validStep =
        (abs(dx) == 1 && dy == 0) ||
        (abs(dy) == 1 && dx == 0);

    if (!validStep)
    {
        autoPath.clear();
        debugPath.clear();
        return;
    }

    // Convert to world direction
    Vector2 nextWorld = CellToWorld(next);
    Vector2 headWorld = player[0].pos;

    Vector2 dir = { nextWorld.x - headWorld.x, nextWorld.y - headWorld.y };

    speed = dir;
    canMove = false;
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

Cell SnakeGame::WorldToCell(Vector2 pos)
{
    Cell c;
    c.x = (int)((pos.x - borderGap.x / 2) / tile_size);
    c.y = (int)((pos.y - borderGap.y / 2) / tile_size);
    return c;
}

Vector2 SnakeGame::CellToWorld(Cell c)
{
    Vector2 pos;
    pos.x = borderGap.x / 2 + c.x * tile_size;
    pos.y = borderGap.y / 2 + c.y * tile_size;
    return pos;
}

int SnakeGame::GetCols()
{
    return (screenW - (int)borderGap.x) / (int)tile_size;
}

int SnakeGame::GetRows()
{
    return (screenH - (int)borderGap.y) / (int)tile_size;
}

bool SnakeGame::IsCellSafe(const Cell& c)
{
    // bounds
    if (c.x < 0 || c.x >= GetCols() || c.y < 0 || c.y >= GetRows())
        return false;

    Vector2 world = CellToWorld(c);

    // body collision (skip head)
    for (int i = 1; i < player.size(); i++)
        if (player[i].pos.x == world.x && player[i].pos.y == world.y)
            return false;

    return true;
}

bool SnakeGame::FindPathAStar(const Cell& start, const Cell& goal, std::vector<Cell>& outPath)
{
    outPath.clear();

    struct Node
    {
        Cell cell;
        float f;
        float g;
    };

    auto cmp = [](const Node& a, const Node& b) { return a.f > b.f; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> open(cmp);

    std::unordered_map<Cell, Cell, CellHash> cameFrom;
    std::unordered_map<Cell, float, CellHash> gScore;
    std::unordered_map<Cell, bool, CellHash> inOpen;

    auto Heuristic = [](const Cell& a, const Cell& b)
        {
            return (float)(abs(a.x - b.x) + abs(a.y - b.y));
        };

    gScore[start] = 0.0f;
    open.push({ start, Heuristic(start, goal), 0.0f });
    inOpen[start] = true;

    while (!open.empty())
    {
        Node current = open.top();
        open.pop();
        inOpen[current.cell] = false;

        if (current.cell == goal)
        {
            Cell c = goal;
            while (!(c == start))
            {
                outPath.push_back(c);
                c = cameFrom[c];
            }
            std::reverse(outPath.begin(), outPath.end());
            return true;
        }

        Cell neighbors[4] = {
            { current.cell.x + 1, current.cell.y },
            { current.cell.x - 1, current.cell.y },
            { current.cell.x,     current.cell.y + 1 },
            { current.cell.x,     current.cell.y - 1 }
        };

        for (const Cell& nb : neighbors)
        {
            if (!IsCellSafe(nb))
                continue;

            float tentativeG = current.g + 1.0f;

            auto it = gScore.find(nb);
            if (it == gScore.end() || tentativeG < it->second)
            {
                cameFrom[nb] = current.cell;
                gScore[nb] = tentativeG;
                float f = tentativeG + Heuristic(nb, goal);

                if (!inOpen[nb])
                {
                    open.push({ nb, f, tentativeG });
                    inOpen[nb] = true;
                }
            }
        }

    }

    return false;
}
