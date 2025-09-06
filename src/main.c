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
    InitWindow(G_WIDTH, G_HEIGHT, "akasha");

    game.screen = LoadRenderTexture(WIDTH, HEIGHT);
    SetTextureFilter(game.screen.texture, TEXTURE_FILTER_POINT);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
    //Drawing to 480x320 render texture
        BeginTextureMode(game.screen);
            ClearBackground(BLACK);
        EndTextureMode();

        //Drawing to frame buffer
        BeginDrawing();
            ClearBackground(WHITE);
            DrawTexturePro(
                game.screen.texture,
                (Rectangle){0, 0, WIDTH, -HEIGHT},
                (Rectangle){0, 0, G_WIDTH, G_HEIGHT},
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        EndDrawing();
    }

    UnloadRenderTexture(game.screen);
    CloseWindow();
}