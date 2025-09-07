#ifndef MAIN_H
#define MAIN_H

#define ASPECT_W 16
#define ASPECT_H 9
#define RENDER_W 1280
#define RENDER_H 960
#define G_DEFAULT_WIDTH 960
#define G_DEFAULT_HEIGHT 640

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
