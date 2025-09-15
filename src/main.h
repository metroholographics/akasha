#ifndef MAIN_H
#define MAIN_H

#define ASPECT_W 16
#define ASPECT_H 9

#define G_DEFAULT_WIDTH 1280
#define G_DEFAULT_HEIGHT 720
#define SPRITE_SIZE 32

#define WORLD_ROWS 48
#define WORLD_COLS 27
#define RENDER_W WORLD_ROWS * SPRITE_SIZE//896
#define RENDER_H WORLD_COLS * SPRITE_SIZE//504
#define WORLD_TILE_W (RENDER_W / WORLD_ROWS)
#define WORLD_TILE_H (RENDER_H / WORLD_COLS)

typedef enum {
    EMPTY = 0,
    CLOUD,
    MOON,
    MOUNTAIN,
    NUM_TILE_TYPES
} TileType;

typedef enum {
    NONE = 0,
    CLOUD_S,
    MOON_S,
    MOUNTAIN_S,
    NUM_SPRITES
} SpriteID;

typedef struct tile {
    TileType type;
} Tile;

typedef struct dimensions {
    int window_width;
    int window_height;
    int screen_width;
    int screen_height;
} Dimensions;

typedef struct game_state {
    RenderTexture2D screen;
    Texture2D spritesheet;
    Dimensions dimensions;
    Tile overworld[WORLD_COLS][WORLD_ROWS];
    Tile* selected_tile;
    Vector2 screen_mouse_pos;
} GameState;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);
void create_tiles(Tile* t_array);
Tile get_tile(TileType t, Tile* t_array);
void create_empty_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Rectangle get_sprite_source(SpriteID index);
void draw_tile(Tile t, float x, float y, Texture2D s);
void set_sprite_ids(Rectangle* s_array);
void create_random_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Vector2 world_to_screen(Vector2 world_pos, Rectangle dest, float scale);

#endif
