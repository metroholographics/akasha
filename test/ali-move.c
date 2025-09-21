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

typedef enum clickstate {
    NONE = 0,
    ORIGIN,
    TARGET,
    OBSTACLE,
    NUM_STATES 
} ClickState;

typedef struct cell {
    Vector2 top_left;
    Vector2 middle;
    ClickState state;
    bool solid;
} Cell;

Cell grid[ROWS][COLS];
Vector2 start_pos;
ClickState state;
Font font;
Cell* target_cell;
Cell* origin_cell;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);
Vector2 get_middle(float x, float y, float w, float h);
void draw_state(ClickState cs);
bool mouse_in_grid(Vector2 pos, Vector2 start, float w, float h);

int main(int argc, char* argv[])
{
    (void)argc; (void)argv;
    InitWindow(WIDTH, HEIGHT, "ali-move");
    SetTargetFPS(120);
    state = ORIGIN;
    target_cell = origin_cell = NULL;

    font = LoadFont("assets/fonts/Kobata-Regular.otf");
    start_pos = get_centered_top_left(GRID_WIDTH, GRID_HEIGHT, WIDTH, HEIGHT);

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Cell* c = &grid[y][x];
            *c = (Cell) {
                .top_left = (Vector2) {
                    .x = start_pos.x + (x * SIZE),
                    .y = start_pos.y + (y * SIZE)
                },
                .solid = false,
                .state = NONE
            };
            c->middle = get_middle(c->top_left.x, c->top_left.y, SIZE, SIZE);
        }
    }

    while(!WindowShouldClose()) {

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
                } else if (state == TARGET) {
                    if (target_cell!= NULL) target_cell->state = NONE;
                    target_cell = &grid[tile_y][tile_x];
                    target_cell->state = TARGET;
                } else {
                    grid[tile_y][tile_x].state = state;
                }
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                grid[tile_y][tile_x].state = NONE;
                if (origin_cell == &grid[tile_y][tile_x]) origin_cell = NULL;
                else if (target_cell == &grid[tile_y][tile_x]) target_cell = NULL;
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
            draw_state(state);
        EndDrawing();
    }
    CloseWindow();
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