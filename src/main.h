#ifndef MAIN_H
#define MAIN_H

#define ASPECT_W 16
#define ASPECT_H 9

#define G_DEFAULT_WIDTH 1280
#define G_DEFAULT_HEIGHT 720
#define SPRITE_SIZE 32

#define WORLD_ROWS 48
#define WORLD_COLS 27
#define RENDER_W WORLD_ROWS * SPRITE_SIZE
#define RENDER_H WORLD_COLS * SPRITE_SIZE
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

typedef struct mouse {
    Vector2 window_pos;
    Vector2 screen_pos;
    Vector2 render_pos;
} Mouse;

typedef struct dimensions {
    Rectangle scaled_screen;
    float scale;
    int window_width;
    int window_height;
    int screen_width;
    int screen_height;
} Dimensions;

typedef struct game_state {
    RenderTexture2D screen;
    Texture2D spritesheet;
    Dimensions dimensions;
    Mouse mouse;
    Tile overworld[WORLD_COLS][WORLD_ROWS];
    Tile* selected_tile;
} GameState;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);
void create_tiles(Tile* t_array);
Tile set_tile(TileType t, Tile* t_array);
void create_empty_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Rectangle get_sprite_source(SpriteID index);
void draw_tile(Tile t, float x, float y, Texture2D s);
void set_sprite_ids(Rectangle* s_array);
void create_random_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Vector2 window_to_screen_coords(Vector2 world_pos, Rectangle dest, float scale);
Vector2 pos_relative_to_render(Vector2 current, Camera2D cam);
void update_game_dimensions(Dimensions* d);
void handle_mouse(Mouse* m, Dimensions* d);
Tile* get_world_tile(Vector2 renderpos, Tile grid[][WORLD_ROWS]);
void draw_world(Tile grid[][WORLD_ROWS]);
void handle_zoom(Mouse* m, Camera2D* cam);

#endif
