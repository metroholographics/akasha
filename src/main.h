#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "astar.h"

#define MAX_TROOPS 50
#define MAX_PATH 576

#define ARMY_MOVE_DELAY 0.2f

typedef enum {
    EMPTY = 0,
    CLOUD,
    MOON,
    MOUNTAIN,
    NUM_TILE_TYPES
} TileType;

typedef enum {
    FREE = 0,
    MOVING,
    NUM_ARMY_STATES
} ArmyState;
typedef enum {
    NONE = 0,
    CLOUD_S,
    MOON_S,
    MOUNTAIN_S,
    NUM_SPRITES
} SpriteID;

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

typedef struct entity {
    bool active;
    bool player;
} Entity;

typedef struct army {
    Entity troops[MAX_TROOPS];
    Entity* commander;
    ast_Node current_path[MAX_PATH];
    int move_tracker;
    int path_length;
    ArmyState state;
} Army;

typedef struct tile {
    int x, y;
    TileType type;
    Army* army;
} Tile;

typedef struct timers {
    float army_move_timer;
} Timers;

typedef struct GameState {
    RenderTexture2D screen;
    Texture2D spritesheet;
    Dimensions dimensions;
    Mouse mouse;
    Tile overworld[WORLD_COLS][WORLD_ROWS];
    Tile* selected_tile;
    Tile* move_tile;
    Army* selected_army;
    Timers timers;
} GameState;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);
void create_tiles(Tile* t_array);
Tile create_tile(TileType tt, Army* a);
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
