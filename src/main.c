#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "main.h"

GameState game;

int main (int argc, char *argv[])
{
    (void)argc; (void)argv;
    game.dimensions.window_width  = G_DEFAULT_WIDTH;
    game.dimensions.window_height = G_DEFAULT_HEIGHT;
    game.dimensions.screen_width  = RENDER_W;
    game.dimensions.screen_height = RENDER_H;

    InitWindow(game.dimensions.window_width, game.dimensions.window_height, "akasha");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    game.screen = LoadRenderTexture(RENDER_W, RENDER_H);
    SetTextureFilter(game.screen.texture, TEXTURE_FILTER_POINT);

    game.spritesheet = LoadTexture("assets/spritesheet.png");

    Vector2 map_pos = get_centered_top_left(WORLD_ROWS * WORLD_TILE_W, WORLD_COLS * WORLD_TILE_H, RENDER_W, RENDER_H);

    SetTargetFPS(120);
    while (!WindowShouldClose()) {

        {
            game.dimensions.window_width  = GetScreenWidth();
            game.dimensions.window_height = GetScreenHeight();
        }
        float scale = fminf((float)game.dimensions.window_width / game.dimensions.screen_width,
            (float)game.dimensions.window_height / game.dimensions.screen_height);
        float screen_w = game.dimensions.screen_width * scale;
        float screen_h = game.dimensions.screen_height * scale;
        Vector2 screen_pos = get_centered_top_left(screen_w, screen_h, game.dimensions.window_width, game.dimensions.window_height);

        //Drawing to render texture
        BeginTextureMode(game.screen);
            ClearBackground(BLACK);
            DrawRectangleLines(map_pos.x, map_pos.y, WORLD_ROWS * WORLD_TILE_W, WORLD_COLS * WORLD_TILE_H, YELLOW);
            for (int y = 0; y < WORLD_COLS; y++) {
                for (int x = 0; x < WORLD_ROWS; x++) {
                    Vector2 rect_pos = (Vector2) {
                        .x = map_pos.x + x * WORLD_TILE_W,
                        .y = map_pos.y + y * WORLD_TILE_H,
                    };
                    DrawRectangleLines(rect_pos.x, rect_pos.y, WORLD_TILE_W, WORLD_TILE_H, DARKGRAY);
                }
            }
            DrawTexturePro(game.spritesheet, (Rectangle){0,0,32,32}, (Rectangle){100,100,WORLD_TILE_W, WORLD_TILE_H}, (Vector2){0,0}, 0.0f, WHITE);
        EndTextureMode();

        //Drawing to frame buffer
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                game.screen.texture,
                (Rectangle){0, 0, RENDER_W, -RENDER_H},
                (Rectangle){screen_pos.x, screen_pos.y, screen_w, screen_h},
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        EndDrawing();
    }

    UnloadRenderTexture(game.screen);
    CloseWindow();
}

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h)
{
    float cen_x = box_w / 2;
    float cen_y = box_h / 2;

    float c_x = cen_x - (0.5f * w);
    float c_y = cen_y - (0.5f * h);

    return (Vector2){c_x, c_y};
}