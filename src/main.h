#ifndef MAIN_H
#define MAIN_H

#define WIDTH 480
#define HEIGHT 320
#define G_WIDTH 1440//(WIDTH * SCALE)
#define G_HEIGHT 960//(HEIGHT * SCALE)

typedef struct game_state {
    RenderTexture2D screen;
} GameState;


#endif
