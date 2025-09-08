#ifndef MAIN_H
#define MAIN_H

#define ASPECT_W 16
#define ASPECT_H 9
#define RENDER_W 960
#define RENDER_H 540
#define G_DEFAULT_WIDTH 1280
#define G_DEFAULT_HEIGHT 720
#define SPRITE_SIZE 32

#define WORLD_ROWS 30
#define WORLD_COLS 18
#define WORLD_TILE_W (RENDER_W / WORLD_ROWS)
#define WORLD_TILE_H (RENDER_H / WORLD_COLS)

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
} GameState;

Vector2 get_centered_top_left(float w, float h, float box_w, float box_h);


#endif
