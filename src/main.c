#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "astar.h"

GameState game;
Tile world_tiles[NUM_TILE_TYPES];
Rectangle sprites[NUM_SPRITES];
Camera2D camera;

int main (int argc, char *argv[])
{
    (void)argc; (void)argv;
    game = (GameState) {0};
    game.dimensions.window_width    = G_DEFAULT_WIDTH;
    game.dimensions.window_height   = G_DEFAULT_HEIGHT;
    game.dimensions.screen_width    = RENDER_W;
    game.dimensions.screen_height   = RENDER_H;
    game.dimensions.scale           = 1.0f;

    set_sprite_ids(sprites);
    create_tiles(world_tiles);
    //create_empty_map(game.overworld, world_tiles);
    create_random_map(game.overworld, world_tiles);

    InitWindow(game.dimensions.window_width, game.dimensions.window_height, "akasha");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetRandomSeed(time(NULL));

    camera          = (Camera2D) {0};
    camera.offset   = (Vector2){RENDER_W / 2, RENDER_H / 2};
    camera.target   = (Vector2){RENDER_W / 2, RENDER_H / 2};
    camera.zoom     = 1.0f;

    game.screen = LoadRenderTexture(RENDER_W, RENDER_H);
    SetTextureFilter(game.screen.texture, TEXTURE_FILTER_POINT);
    game.spritesheet = LoadTexture("assets/spritesheet.png");
    SetTextureFilter(game.spritesheet, TEXTURE_FILTER_POINT);

    Entity player = (Entity) {
        .sprite = PLAYER_S,
        .active = true,
        .player = true
    };
    Entity enemy = (Entity) {
        .sprite = ENEMY_S,
        .active = true,
        .player = false
    };

    create_army(&game.armies, &player, 5, 1);
    create_army(&game.armies, &enemy, 10, 10);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Dimensions* g_d = &game.dimensions;
        Mouse*      g_m = &game.mouse;
        update_game_dimensions(g_d);

        Army* a = game.selected_army;
        if (a != NULL && a->state == MOVING) {
            game.timers.army_move_timer += dt;
            if (game.timers.army_move_timer >= ARMY_MOVE_DELAY) {
                int n = a->move_tracker + 1;
                ast_Node current = a->current_path[a->move_tracker];
                ast_Node next = a->current_path[n];
                if (game.overworld[next.y][next.x].army != NULL) {
                    printf("Battle!\n");
                    //::TODO: Add battle code. this just blocks movement for now
                    a->move_tracker = a->path_length;
                    goto end_army_move;
                }
                game.overworld[current.y][current.x].army = NULL;
                game.overworld[next.y][next.x].army = a;
                a->move_tracker++;
                game.timers.army_move_timer = 0.0f;
                game.selected_tile = &game.overworld[next.y][next.x];
            }
            end_army_move:
                if (a->move_tracker >= a->path_length - 1) {
                    a->move_tracker = 0;
                    a->path_length = 0;
                    a->state = (ArmyState) FREE;
                    game.timers.army_move_timer = 0.0f;
                }
        } else {
            handle_mouse(g_m, g_d);
        }
        //Drawing to render texture
        BeginTextureMode(game.screen);
            BeginMode2D(camera);
            ClearBackground((Color){25,0,25,255});

            draw_world(game.overworld);
            // DrawCircle(camera.target.x, camera.target.y, 5, RED);
            // DrawCircle(camera.offset.x, camera.offset.y, 5, PINK);
            // DrawCircle(g_d->scaled_screen.x, g_d->scaled_screen.y, 3, BLUE);
            // DrawRectangleLines(g_d->scaled_screen.x, g_d->scaled_screen.y, g_d->scaled_screen.width, g_d->scaled_screen.height, BLUE);
            EndMode2D();
        EndTextureMode();

        //Drawing to frame buffer
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(
                game.screen.texture,
                (Rectangle){0, 0, RENDER_W, -RENDER_H},
                g_d->scaled_screen,
                (Vector2){0,0}, 0.0f, WHITE
            );
        EndDrawing();
    }

    UnloadRenderTexture(game.screen);
    CloseWindow();
    return 0;
}

void draw_world(Tile grid[][WORLD_ROWS])
{
    for (int y = 0; y < WORLD_COLS; y++) {
        for (int x = 0; x < WORLD_ROWS; x++) {
            Tile* t = &grid[y][x];
            Vector2 tile_pos = (Vector2) {
                .x = x * WORLD_TILE_W,
                .y = y * WORLD_TILE_H,
            };
            if (t == game.selected_tile) {
                DrawRectangle(tile_pos.x, tile_pos.y, WORLD_TILE_W, WORLD_TILE_H, DARKPURPLE);
            } else if (t == game.move_tile) {
                DrawRectangle(tile_pos.x, tile_pos.y, WORLD_TILE_W, WORLD_TILE_H, DARKGREEN);
            }
            draw_tile(*t, tile_pos.x, tile_pos.y, game.spritesheet);

            if (t->army != NULL) {
                draw_army(*t->army, tile_pos.x, tile_pos.y, game.spritesheet);
            }
        }
        
    }
    Army* a = game.selected_army;
    if (a != NULL && a->path_length > 0) {
        for (int i = a->move_tracker + 1; i < a->path_length; i++) {
            ast_Node step = a->current_path[i];
            DrawCircle(step.x * WORLD_TILE_W + HALF_WORLD_TILE_W, step.y * WORLD_TILE_H + HALF_WORLD_TILE_H, 2, GREEN);
        }
    }
}

void draw_tile(Tile t, float x, float y, Texture2D s)
{
    Rectangle dest = (Rectangle) {
        .height = WORLD_TILE_H,
        .width = WORLD_TILE_W,
        .x = x,
        .y = y
     };
    DrawTexturePro(s, get_sprite_source(t.sprite), dest, (Vector2){0,0}, 0.0f, WHITE);
}

void draw_army(Army a, float x, float y, Texture2D s)
{
    Rectangle dest = (Rectangle) {
        .height = WORLD_TILE_H,
        .width = WORLD_TILE_W,
        .x = x,
        .y = y
     };
    DrawTexturePro(s, get_sprite_source(a.commander->sprite), dest, (Vector2){0,0}, 0.0f, WHITE);
}

void create_army(ArmyManager* am, Entity* commander, int x, int y)
{
    if (am->army_count >= MAX_ARMIES) {
        printf(">>>reached max armies!\n");
        return;
    }
    for (int i = 0; i < MAX_ARMIES; i++) {
        if (!am->armies[i].initialised) {
            am->armies[i] = (Army) {0};
            am->armies[i].initialised   = true;
            am->armies[i].commander     = commander;
            am->armies[i].state         = (ArmyState) FREE;
            am->army_count++;
            game.overworld[y][x].army = &am->armies[i];
            break;
        }
    }
}


void handle_mouse(Mouse* m, Dimensions* d)
{
    m->window_pos = GetMousePosition();
    m->screen_pos = window_to_screen_coords(m->window_pos, d->scaled_screen, d->scale);
    m->render_pos = pos_relative_to_render(m->screen_pos, camera);
    handle_zoom(m, &camera);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        game.selected_tile  = NULL;
        game.move_tile      = NULL;
        if (game.selected_army != NULL) {
            game.selected_army->path_length = 0;
            game.selected_army = NULL;
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Tile* t         = get_world_tile(m->render_pos, game.overworld);
        Tile* selected  = game.selected_tile;
        bool available  = (t != NULL);
        if (available) {
            bool already_selected   = (t == selected);
            bool selected_move_tile = (t == game.move_tile);
            bool selected_army      = (selected != NULL && selected->army != NULL &&
                selected->army->commander->active && selected->army->commander->player
            );
            if (!already_selected && !selected_army) {
                game.selected_tile = t;
            } else if (!already_selected && selected_army && !selected_move_tile) {
                game.selected_army = selected->army;
                game.move_tile = t;
                ast_Node start = (ast_Node) {selected->x, selected->y};
                ast_Node goal  = (ast_Node) {t->x, t->y};
                selected->army->path_length = find_astar_path(&game, start, goal, selected->army->current_path, MAX_PATH);
            } else if (selected_move_tile) {
                game.selected_army->state = MOVING;
                game.timers.army_move_timer = 0.0f;
                game.move_tile = NULL;
            }
        }
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }
}

void handle_zoom(Mouse* m, Camera2D* cam)
{
    float move = GetMouseWheelMove();
    if (move != 0.0f) {
        Vector2 worldBefore = pos_relative_to_render(m->window_pos, *cam);

        cam->zoom = expf(logf(cam->zoom) + ((float)GetMouseWheelMove()*0.1f));
        if (cam->zoom > 3.0f) cam->zoom = 3.0f;
        Vector2 worldAfter = pos_relative_to_render(m->window_pos, *cam);
        
        cam->target.x += (worldBefore.x - worldAfter.x);
        cam->target.y += (worldBefore.y - worldAfter.y);
        
    }
}

Tile* get_world_tile(Vector2 renderpos, Tile grid[][WORLD_ROWS])
{
    int tile_x = renderpos.x / WORLD_TILE_W;
    int tile_y = renderpos.y / WORLD_TILE_H;
    if (tile_x >= 0 && tile_x < WORLD_ROWS && tile_y >= 0 && tile_y < WORLD_COLS) {
        return &grid[tile_y][tile_x];
    }
    return NULL;
}

Vector2 pos_relative_to_render(Vector2 current, Camera2D cam)
{
    return (Vector2) {
        (current.x - cam.offset.x) / cam.zoom + cam.target.x,
        (current.y - cam.offset.y) / cam.zoom + cam.target.y
    };
}

void update_game_dimensions(Dimensions* d)
{
    d->window_width  = GetScreenWidth();
    d->window_height = GetScreenHeight();

    d->scale = fminf(
        (float)d->window_width  / d->screen_width,
        (float)d->window_height / d->screen_height
    );

    int screen_w = d->screen_width  * d->scale;
    int screen_h = d->screen_height * d->scale;

    d->scaled_screen = (Rectangle) {
        .x      = (d->window_width  - screen_w) * 0.5f,
        .y      = (d->window_height - screen_h) * 0.5f,
        .width  = screen_w,
        .height = screen_h
    };
}

Vector2 window_to_screen_coords(Vector2 world_pos, Rectangle dest, float scale)
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
    t_array[EMPTY]      = create_tile(EMPTY, NULL, 1, NONE_S);
    t_array[CLOUD]      = create_tile(CLOUD, NULL, 1, CLOUD_S);
    t_array[MOON]       = create_tile(MOON, NULL, 1, MOON_S);
    t_array[MOUNTAIN]   = create_tile(MOUNTAIN, NULL, 2, MOUNTAIN_S);
    t_array[FOREST]     = create_tile(FOREST, NULL, 1, FOREST_S);
    return;
}

Tile create_tile(TileType tt, Army* a, int cost, SpriteID s)
{
    Tile t = {0};
    t.type      = tt;
    t.army      = a;
    t.move_cost = cost;
    t.sprite    = s;

    return t;
}

Tile set_tile(TileType t, Tile* t_array)
{
    return t_array[t];
}

void create_empty_map(Tile grid[][WORLD_ROWS], Tile* t_array)
{
    for (int y = 0; y < WORLD_COLS; y++) {
        for (int x = 0; x < WORLD_ROWS; x++) {
            grid[y][x] = set_tile(EMPTY, t_array);
            grid[y][x].x = x;
            grid[y][x].y = y;
        }
    }
}

void create_random_map(Tile grid[][WORLD_ROWS], Tile* t_array)
{
    TileType t;
    for (int y = 0; y < WORLD_COLS; y++) {
        for (int x = 0; x < WORLD_ROWS; x++) {
            t = GetRandomValue(EMPTY, NUM_TILE_TYPES - 1);
            grid[y][x] = set_tile(t, t_array);
            grid[y][x].x = x;
            grid[y][x].y = y;
        }
    }
}

void set_sprite_ids(Rectangle* s_array)
{
    s_array[NONE_S]     = (Rectangle) {0,0,0,0};
    s_array[PLAYER_S]   = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 0, .y = 0};
    s_array[ENEMY_S]    = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 0, .y = 32};
    s_array[CLOUD_S]    = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 32, .y = 0};
    s_array[MOON_S]     = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 64, .y = 0};
    s_array[MOUNTAIN_S] = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 96, .y = 0};
    s_array[FOREST_S]   = (Rectangle) {
        .height = SPRITE_SIZE, .width = SPRITE_SIZE,.x = 128, .y = 0};
    return;
}
