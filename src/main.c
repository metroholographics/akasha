#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "main.h"

GameState game;
Tile world_tiles[NUM_TILE_TYPES];
Rectangle sprites[NUM_SPRITES];
Camera2D camera;

int main (int argc, char *argv[])
{
    (void)argc; (void)argv;
    game.dimensions.window_width  = G_DEFAULT_WIDTH;
    game.dimensions.window_height = G_DEFAULT_HEIGHT;
    game.dimensions.screen_width  = RENDER_W;
    game.dimensions.screen_height = RENDER_H;

    set_sprite_ids(sprites);
    create_tiles(world_tiles);
    //create_empty_map(game.overworld, world_tiles);
    create_random_map(game.overworld, world_tiles);

    InitWindow(game.dimensions.window_width, game.dimensions.window_height, "akasha");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetRandomSeed(time(NULL));

    camera = (Camera2D) {0};
    camera.offset = (Vector2){RENDER_W / 2, RENDER_H / 2};
    camera.target = (Vector2){RENDER_W / 2, RENDER_H / 2};
    camera.zoom = 1.0f;

    game.screen = LoadRenderTexture(RENDER_W, RENDER_H);
    SetTextureFilter(game.screen.texture, TEXTURE_FILTER_POINT);

    game.spritesheet = LoadTexture("assets/spritesheet.png");
    SetTextureFilter(game.spritesheet, TEXTURE_FILTER_POINT);

    SetTargetFPS(120);
    while (!WindowShouldClose()) {
        game.dimensions.window_width  = GetScreenWidth();
        game.dimensions.window_height = GetScreenHeight();

        float scale = fminf(
            (float)game.dimensions.window_width  / game.dimensions.screen_width,
            (float)game.dimensions.window_height / game.dimensions.screen_height
        );

        int screen_w = game.dimensions.screen_width  * scale;
        int screen_h = game.dimensions.screen_height * scale;

        Rectangle window_dest = (Rectangle) {
            .x = (game.dimensions.window_width  - screen_w) * 0.5f,
            .y = (game.dimensions.window_height - screen_h) * 0.5f,
            .width  = screen_w,
            .height = screen_h
        };
        Vector2 world_mouse_pos = GetMousePosition();
        game.screen_mouse_pos   = world_to_screen(world_mouse_pos, window_dest, scale);
        printf("%f %f\n", game.screen_mouse_pos.x, game.screen_mouse_pos.y);
        float move = GetMouseWheelMove();
        if (move != 0.0f) {
            Vector2 worldBefore = {
                (world_mouse_pos.x - camera.offset.x) / camera.zoom + camera.target.x,
                (world_mouse_pos.y - camera.offset.y) / camera.zoom + camera.target.y,
            };
            camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));
            if (camera.zoom > 3.0f) camera.zoom = 3.0f;
            Vector2 worldAfter = {
                (world_mouse_pos.x - camera.offset.x) / camera.zoom + camera.target.x,
                (world_mouse_pos.y - camera.offset.y) / camera.zoom + camera.target.y,
            };
            
            camera.target.x += (worldBefore.x - worldAfter.x);
            camera.target.y += (worldBefore.y - worldAfter.y);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        //printf("%f %f\n", move, camera.zoom);
        //Drawing to render texture
        BeginTextureMode(game.screen);
            BeginMode2D(camera);
            ClearBackground((Color){25,0,25,255});
            for (int y = 0; y < WORLD_COLS; y++) {
                for (int x = 0; x < WORLD_ROWS; x++) {
                    Tile t = game.overworld[y][x];
                    Vector2 tile_pos = (Vector2) {
                        .x = x * WORLD_TILE_W,
                        .y = y * WORLD_TILE_H,
                    };
                    draw_tile(t, tile_pos.x, tile_pos.y, game.spritesheet);
                    //DrawRectangleLines(tile_pos.x, tile_pos.y, WORLD_TILE_W, WORLD_TILE_H, DARKGRAY);
                }
            }
            DrawTexturePro(
                game.spritesheet,
                (Rectangle){0,0,32,32},
                (Rectangle){100,100,WORLD_TILE_W, WORLD_TILE_H},
                (Vector2){0,0}, 0.0f, WHITE
            );
            // DrawCircle(camera.target.x, camera.target.y, 5, RED);
            // DrawCircle(camera.offset.x, camera.offset.y, 5, PINK);
            EndMode2D();
        EndTextureMode();

        //Drawing to frame buffer
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                game.screen.texture,
                (Rectangle){0, 0, RENDER_W, -RENDER_H},
                window_dest,
                (Vector2){0,0}, 0.0f, WHITE
            );
        EndDrawing();
    }

    UnloadRenderTexture(game.screen);
    CloseWindow();
    return 0;
}

Vector2 world_to_screen(Vector2 world_pos, Rectangle dest, float scale)
{
    float mx = world_pos.x - dest.x;
    float my = world_pos.y - dest.y;

    if (mx < 0 || my < 0 || mx >= dest.width || my >= dest.height) {
        return (Vector2) {-1,-1};
    }

    return (Vector2) {
        .x = mx / scale,
        .y = my / scale
    };
}

void draw_tile(Tile t, float x, float y, Texture2D s)
{
    SpriteID idx;
    Rectangle dest = (Rectangle) {
        .height = WORLD_TILE_H,
        .width = WORLD_TILE_W,
        .x = x,
        .y = y
     };

    switch (t.type) {
        case EMPTY:     idx = 0;            break;
        case CLOUD:     idx = CLOUD_S;      break;
        case MOON:      idx = MOON_S;       break;
        case MOUNTAIN:  idx = MOUNTAIN_S;   break;
        default:        idx = 0;            break;
    };
    DrawTexturePro(s, get_sprite_source(idx), dest, (Vector2){0,0}, 0.0f, WHITE);
}

Rectangle get_sprite_source(SpriteID index)
{
    return sprites[index];
}

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h)
{
    float cen_x = box_w / 2;
    float cen_y = box_h / 2;

    float c_x = cen_x - (0.5f * w);
    float c_y = cen_y - (0.5f * h);

    return (Vector2){c_x, c_y};
}

void create_tiles(Tile* t_array)
{
    t_array[EMPTY]      = (Tile) {
        .type = EMPTY
    };
    t_array[CLOUD]      = (Tile) {
        .type = CLOUD
    };
    t_array[MOON]       = (Tile) {
        .type = MOON
    };
    t_array[MOUNTAIN]   = (Tile) {
        .type = MOUNTAIN
    };
    return;
}

Tile get_tile(TileType t, Tile* t_array)
{
    return t_array[t];
}

void create_empty_map(Tile grid[][WORLD_ROWS], Tile* t_array)
{
    for (int y = 0; y < WORLD_COLS; y++) {
        for (int x = 0; x < WORLD_ROWS; x++) {
            grid[y][x] = get_tile(EMPTY, t_array);
        }
    }
}

void create_random_map(Tile grid[][WORLD_ROWS], Tile* t_array)
{
    TileType t;
    for (int y = 0; y < WORLD_COLS; y++) {
        for (int x = 0; x < WORLD_ROWS; x++) {
            t = GetRandomValue(EMPTY, NUM_TILE_TYPES - 1);
            grid[y][x] = get_tile(t, t_array);
        }
    }
}

void set_sprite_ids(Rectangle* s_array)
{
    s_array[NONE]       = (Rectangle) {0,0,0,0};
    s_array[CLOUD_S]    = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 32, .y = 0};
    s_array[MOON_S]     = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 64, .y = 0};
    s_array[MOUNTAIN_S] = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 96, .y = 0};
    return;
}
