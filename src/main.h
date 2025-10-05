#ifndef MAIN_H
#define MAIN_H

#include "config.h"

#define MAX_TROOPS 50
#define MAX_PATH 576

#define MAX_ARMIES 32
#define ARMY_MOVE_DELAY 0.2f

typedef struct Army Army;

typedef enum {
    EMPTY = 0,
    CLOUD,
    MOON,
    MOUNTAIN,
    FOREST,
    NUM_TILE_TYPES
} TileType;

typedef enum {
    NONE = 0,
    MAGE,
    SOLDIER,
    ARCHER,
    NUM_ENTITY_TYPES,
} EntityType;

typedef enum {
    COMMANDER = 0,
    BACK,
    MIDDLE,
    FRONT,
    NUM_BATTLE_RANKS
} BattleRank;

typedef enum {
    FREE = 0,
    MOVING,
    NUM_ARMY_STATES
} ArmyState;

typedef enum {
    NONE_S = 0,
    PLAYER_S,
    ENEMY_S,
    CLOUD_S,
    MOON_S,
    MOUNTAIN_S,
    FOREST_S,
    MAGE_S, 
    SOLDIER_S,
    ARCHER_S,
    NUM_SPRITES
} SpriteID;

typedef enum {
    MAIN_MENU = 0,
    OVERWORLD,
    BATTLE_MODE,
    NUM_GAME_SCREENS
} GameScreen;

typedef struct ast_Node {
    int x;
    int y;
} ast_Node;

typedef struct ast_astNode {
    ast_Node    node;
    int         g, h, f;
    ast_Node    parent;
    bool        used;
} ast_astNode;

typedef struct mouse {
    Vector2 window_pos;
    Vector2 screen_pos;
    Vector2 render_pos;
} Mouse;

typedef struct dimensions {
    Rectangle   scaled_screen;
    float       scale;
    int         window_width;
    int         window_height;
    int         screen_width;
    int         screen_height;
} Dimensions;

typedef struct entity {
    SpriteID    sprite;
    EntityType  type;
    bool        active;
    bool        player;
    Army*       army;
    BattleRank  rank;
} Entity;

typedef struct Army {
    bool        initialised;
    Entity      troops[MAX_TROOPS];
    Entity*     commander;
    ast_Node    current_path[MAX_PATH];
    int         move_tracker;
    int         path_length;
    ArmyState   state;
    int         troop_count;
} Army;

typedef struct ArmyManager {
    Army    armies[MAX_ARMIES];
    int     army_count;
} ArmyManager;

typedef struct tile {
    SpriteID    sprite;
    int         x, y;
    TileType    type;
    Army*       army;
    int         move_cost;
} Tile;

typedef struct timers {
    float army_move_timer;
} Timers;

typedef struct BattleManager {
    Army*   army1;
    Army*   army2;
    Tile    t;
    bool    active;
    Entity  army1_troops[MAX_TROOPS];
    Entity  army2_troops[MAX_TROOPS];
    Vector2 army1_pos[MAX_TROOPS];
    Vector2 army2_pos[MAX_TROOPS];
} BattleManager;

typedef struct GameState {
    RenderTexture2D screen;
    Texture2D       spritesheet;
    Dimensions      dimensions;
    Mouse           mouse;
    Tile            overworld[WORLD_COLS][WORLD_ROWS];
    ArmyManager     armies;
    BattleManager   battle;
    Tile*           selected_tile;
    Tile*           move_tile;
    Army*           selected_army;
    Timers          timers;
    GameScreen      game_screen;
} GameState;

Vector2     get_centered_top_left(float w, float h, float box_w, float box_h);
void        create_tiles(Tile* t_array);
Tile        create_tile(TileType tt, Army* a, int cost, SpriteID s);
Tile        set_tile(TileType t, Tile* t_array);
void        create_entities(Entity* e_array);
void        create_empty_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Rectangle   get_sprite_source(SpriteID index);
void        draw_tile(Tile t, float x, float y, Texture2D s);
void        draw_entity(Entity e, float x, float y, Texture2D s);
void        set_sprite_ids(Rectangle* s_array);
void        create_random_map(Tile grid[][WORLD_ROWS], Tile* t_array);
Vector2     window_to_screen_coords(Vector2 world_pos, Rectangle dest, float scale);
Vector2     pos_relative_to_render(Vector2 current, Camera2D cam);
void        update_game_dimensions(Dimensions* d);
void        handle_mouse(Mouse* m, Dimensions* d);
Tile*       get_world_tile(Vector2 renderpos, Tile grid[][WORLD_ROWS]);
void        draw_world(Tile grid[][WORLD_ROWS]);
void        handle_zoom(Mouse* m, Camera2D* cam);
void        create_army(ArmyManager* am, Entity* commander, int x, int y);
void        add_to_army(Entity* e, EntityType et, int amount);
void        set_battle_state(BattleManager* b, Army* a1, Army* a2, Tile t);
void        update_battle(BattleManager* b);
void        draw_battle(BattleManager* b);
void        create_battle_troops(Army a, Entity* troops);
void        set_troop_positions(Entity* troops, Vector2* positions, bool left);

#endif
