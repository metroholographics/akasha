#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define FONT_SIZE 28
#define WIDTH 800
#define HEIGHT 600
#define ROWS 15
#define COLS 20
#define SIZE 30

#define GRID_WIDTH (SIZE * COLS)
#define GRID_HEIGHT (SIZE * ROWS)

#define ORIGIN_COL DARKGREEN
#define TARGET_COL DARKBLUE
#define OBSTACLE_COL DARKPURPLE

#define MAX_NODES (COLS * ROWS)
#define MAX_PATH 512

typedef struct node {
    int x, y;
} Node;

typedef struct astarnode {
    Node node;
    int g, h, f;
    Node parent;
    bool used;
} AStarNode;

static inline int heuristic(Node a, Node b) {
    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    int h  = (dx > dy) ? dx : dy;
    return h * 10 + (dx + dy);
}

static inline bool in_bounds(int x, int y) {
    return (x >= 0 && y >= 0 && x < COLS && y < ROWS);
}

static const int dirs[8][2] = {
    {1,0}, {-1,0}, {0,1} , {0, -1},
    {1,1}, {-1,1}, {1,-1}, {-1,-1}
};

typedef enum clickstate {
    NONE = 0,
    ORIGIN,
    TARGET,
    OBSTACLE,
    NUM_STATES 
} ClickState;

typedef struct cell {
    Vector2 id;
    Vector2 top_left;
    Vector2 middle;
    ClickState state;
    int cost;
} Cell;

typedef struct simulation {
    Vector2 current_cell;
    Vector2 target_cell;
    float timer;
    bool sim;
    bool draw_line;
} Simulation;

Cell grid[ROWS][COLS];
Vector2 start_pos;
ClickState state;
Font font;
Cell* target_cell;
Cell* origin_cell;
Simulation simulation;
Node currentPath[MAX_PATH];
int currentPathLen = 0;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);
Vector2 get_middle(float x, float y, float w, float h);
void draw_state(ClickState cs);
bool mouse_in_grid(Vector2 pos, Vector2 start, float w, float h);

int find_astar_path(Cell grid[][COLS], Node start, Node goal, Node outpath[], int maxPath);

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;
    InitWindow(WIDTH, HEIGHT, "playground");
    SetTargetFPS(120);
    state = ORIGIN;
    target_cell = origin_cell = NULL;

    font = LoadFont("assets/fonts/Kobata-Regular.otf");
    start_pos = get_centered_top_left(GRID_WIDTH, GRID_HEIGHT, WIDTH, HEIGHT);

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Cell* c = &grid[y][x];
            *c = (Cell) {
                .id = (Vector2) {x, y},
                .top_left = (Vector2) {
                    .x = start_pos.x + (x * SIZE),
                    .y = start_pos.y + (y * SIZE)
                },
                .cost = 1,
                .state = NONE
            };
            c->middle = get_middle(c->top_left.x, c->top_left.y, SIZE, SIZE);
        }
    }
    simulation.sim          =  false;
    simulation.timer        = 0.0f;
    simulation.current_cell = (Vector2) {-1,-1};
    simulation.target_cell  = (Vector2) {-1,-1};

    while(!WindowShouldClose()) {
        float delta = GetFrameTime();
        Vector2 mouse_pos = GetMousePosition();
        float move = GetMouseWheelMove();

        if (move > 0.0f) {
            state = (state + 1) % NUM_STATES;
        } else if (move < 0.0f) {
            state = (state - 1 + NUM_STATES) % NUM_STATES;
        }

        if (mouse_in_grid(mouse_pos, start_pos, GRID_WIDTH, GRID_HEIGHT)) {
            int tile_x =  (mouse_pos.x - start_pos.x) / SIZE;
            int tile_y =  (mouse_pos.y - start_pos.y) / SIZE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (state == ORIGIN) {
                    if (origin_cell != NULL) origin_cell->state = NONE;
                    origin_cell = &grid[tile_y][tile_x];
                    origin_cell->state = ORIGIN;
                    simulation.current_cell = (Vector2) {
                        .x = tile_x,
                        .y = tile_y,
                    };
                } else if (state == TARGET) {
                    if (target_cell!= NULL) target_cell->state = NONE;
                    target_cell = &grid[tile_y][tile_x];
                    target_cell->state = TARGET;
                    simulation.target_cell = (Vector2) {
                        .x = tile_x,
                        .y = tile_y,
                    };
                } else {
                    grid[tile_y][tile_x].state = state;
                }
                switch (state) {
                    case NONE:
                    case TARGET:
                    case ORIGIN:
                        grid[tile_y][tile_x].cost = 1;
                        break;
                    case OBSTACLE:
                        grid[tile_y][tile_x].cost = 100;
                        break;
                    default:
                        break;
                }
                printf("%d %d: %d\n", tile_x, tile_y, grid[tile_y][tile_x].cost);

            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                grid[tile_y][tile_x].state  = NONE;
                grid[tile_y][tile_x].cost   = 1;
                if (origin_cell == &grid[tile_y][tile_x]) origin_cell       = NULL;
                else if (target_cell == &grid[tile_y][tile_x]) target_cell  = NULL;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            simulation.sim = !simulation.sim;
        }

        if (simulation.sim) {
            simulation.timer += delta;
            bool target_active = (target_cell != NULL);
            bool origin_active = (origin_cell != NULL);
            bool find_path = (target_active && origin_active);
            simulation.draw_line = find_path;

            if (find_path) {
                Node start  = {origin_cell->id.x, origin_cell->id.y};
                Node end    = {target_cell->id.x, target_cell->id.y};
                currentPathLen = find_astar_path(grid, start, end, currentPath, MAX_PATH);
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            for (int y = 0; y < ROWS; y++) {
                for (int x = 0; x < COLS; x++) {
                    Cell c = grid[y][x];
                    if (c.state == ORIGIN) {
                        DrawRectangle(c.top_left.x, c.top_left.y, SIZE, SIZE, ORIGIN_COL);
                    } else if (c.state == TARGET) {
                        DrawRectangle(c.top_left.x, c.top_left.y, SIZE, SIZE, TARGET_COL);
                    } else if (c.state == OBSTACLE) {
                        DrawRectangle(c.top_left.x, c.top_left.y, SIZE, SIZE, OBSTACLE_COL);
                    } else {
                        DrawRectangleLines(c.top_left.x, c.top_left.y, SIZE, SIZE, DARKGRAY);
                    }

                    DrawCircle(c.middle.x, c.middle.y, 1, DARKGRAY);
                }
            }
            if (simulation.draw_line) {
                //DrawLine(origin_cell->middle.x, origin_cell->middle.y, target_cell->middle.x, target_cell->middle.y, YELLOW);
            }
            if (simulation.sim) {
                for (int i = 1; i < currentPathLen - 1; i++) {
                    Node step = currentPath[i];
                    DrawRectangle(start_pos.x + step.x * SIZE, start_pos.y + step.y * SIZE, SIZE, SIZE, Fade(RED, 0.5f));
                }
            }
            draw_state(state);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

int find_astar_path(Cell grid[][COLS], Node start, Node goal, Node outpath[], int maxPath)
{
    AStarNode nodes[MAX_NODES]  = {0};
    bool closed[ROWS][COLS]     = {0};

    int startIdx = start.y * COLS + start.x;
    nodes[startIdx].node    = start;
    nodes[startIdx].g       = 0;
    nodes[startIdx].h       = heuristic(start, goal);
    nodes[startIdx].f       = nodes[startIdx].h;
    nodes[startIdx].parent  = (Node){-1,-1};
    nodes[startIdx].used    = true;

    Node openSet[MAX_NODES];
    int openCount = 0;
    openSet[openCount++] = start;

    while (openCount > 0) {
        int bestIdx = 0;
        for (int i = 1; i < openCount; i++) {
            Node n = openSet[i];
            int idx = n.y * COLS + n.x;
            int bestNodeIdx = openSet[bestIdx].y * COLS + openSet[bestIdx].x;
            if (nodes[idx].f < nodes[bestNodeIdx].f) bestIdx = i;
        }

        Node current = openSet[bestIdx];
        int currentIdx = current.y * COLS + current.x;
        openSet[bestIdx] = openSet[--openCount];

        if (current.x == goal.x && current.y == goal.y) {
            Node pathBuf[MAX_NODES];
            int pathLen = 0;
            Node step = goal;

            while (!(step.x == -1 && step.y == -1)) {
                pathBuf[pathLen++] = step;
                int idx = step.y * COLS + step.x;
                step = nodes[idx].parent;
            }

            int count = (pathLen < maxPath) ? pathLen : maxPath;
            for (int i = 0; i < count; i++) {
                outpath[i] = pathBuf[pathLen - 1 - i];
            }
            return count;
        }

        closed[current.y][current.x] = true;

        for (int d = 0; d < 8; d++) {
            int nx = current.x + dirs[d][0];
            int ny = current.y + dirs[d][1];
            if (!in_bounds(nx, ny)) continue;
            //if (!grid[ny][nx].walkable) continue;
            if (closed[ny][nx]) continue;

            Node neighbour = {nx, ny};
            int nIdx = ny * COLS + nx;
            int moveCost = grid[ny][nx].cost;
            int tentativeG = nodes[currentIdx].g + moveCost;

            if (!nodes[nIdx].used || tentativeG < nodes[nIdx].g) {
                nodes[nIdx].used = true;
                nodes[nIdx].g = tentativeG;
                nodes[nIdx].h = heuristic(neighbour, goal);
                nodes[nIdx].f = nodes[nIdx].g + nodes[nIdx].h;
                nodes[nIdx].parent = current;

                bool inOpen = false;
                for (int i = 0; i < openCount; i++) {
                    if (openSet[i].x == nx && openSet[i].y == ny) {
                        inOpen = true;
                        break;
                    }
                }
                if (!inOpen) {
                    openSet[openCount++] = neighbour;
                }
            }
        }
    }
    return 0;
}


bool mouse_in_grid(Vector2 pos, Vector2 start, float w, float h)
{
    return (
        pos.x >= start.x &&
        pos.x < start.x + w &&
        pos.y >= start.y &&
        pos.y < start.y + h
    );
}

void draw_state(ClickState cs) {
    Vector2 pos = (Vector2) {.x = 25, .y = 25};
    float spacing = 1.5f;
    switch (cs) {
        case ORIGIN:
            DrawTextEx(font, "ORIGIN", pos, FONT_SIZE, spacing, ORIGIN_COL);
            break;
        case TARGET:
            DrawTextEx(font, "TARGET", pos, FONT_SIZE, spacing, TARGET_COL);
            break;
        case OBSTACLE:
            DrawTextEx(font, "OBSTACLE", pos, FONT_SIZE, spacing, OBSTACLE_COL);
            break;
        default:
            DrawTextEx(font, "NONE", pos, FONT_SIZE, spacing, WHITE);
            break;
    }
}

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h)
{
    float cen_x = box_w / 2;
    float cen_y = box_h / 2;

    float c_x = cen_x - (0.5f * w);
    float c_y = cen_y - (0.5f * h);

    return (Vector2){c_x, c_y};
}

Vector2 get_middle(float x, float y, float w, float h)
{
    return (Vector2) {
        .x = x + 0.5 * w,
        .y = y + 0.5 * h
    };
}