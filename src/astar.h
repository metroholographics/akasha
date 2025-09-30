#ifndef ASTAR_H
#define ASTAR_H

#include <math.h>
#include "config.h"
#include <stdbool.h>

#define AST_MAX_NODES (WORLD_ROWS * WORLD_COLS)
#define AST_MAX_PATH 576

typedef struct ast_Node {
    int x;
    int y;
} ast_Node;

typedef struct ast_astNode {
    ast_Node node;
    int g, h, f;
    ast_Node parent;
    bool used;
} ast_astNode;

static inline int ast_heuristic(ast_Node a, ast_Node b)
{
    int dx  = abs(a.x - b.x);
    int dy  = abs(a.y - b.y);
    int h   = (dx > dy) ? dx : dy;
    return (h * 10 + (dx + dy));
}

static inline bool in_bounds(int x, int y)
{
    return (x >= 0 && x < WORLD_ROWS && y >= 0 && y < WORLD_COLS);
}

static const int ast_dirs[8][2] = {
    {1,0}, {-1,0}, {0,1} , {0, -1},
    {1,1}, {-1,1}, {1,-1}, {-1,-1}
};

struct GameState;

int find_astar_path(struct GameState *g, ast_Node start, ast_Node goal, ast_Node outpath[], int maxpath)
{
    ast_astNode nodes[AST_MAX_NODES]    = {0};
    bool closed[WORLD_COLS][WORLD_ROWS] = {0};

    int startIdx            = start.y * WORLD_ROWS + start.x;
    nodes[startIdx].node    = start;
    nodes[startIdx].g       = 0;
    nodes[startIdx].h       = ast_heuristic(start, goal);
    nodes[startIdx].f       = nodes[startIdx].h;
    nodes[startIdx].parent  = (ast_Node) {.x = -1, .y = -1};
    nodes[startIdx].used    = true;

    ast_Node openSet[AST_MAX_NODES];
    int openCount = 0;
    openSet[openCount++] = start;

    while (openCount > 0) {
        int bestIdx = 0;
        for (int i = 1; i < openCount; i++) {
            ast_Node n = openSet[i];
            int idx = n.y * WORLD_ROWS + n.x;
            int bestNodeIdx = openSet[bestIdx].y * WORLD_ROWS + openSet[bestIdx].x;
            if (nodes[idx].f < nodes[bestNodeIdx].f) bestIdx = i;
        }

        ast_Node current = openSet[bestIdx];
        int currentIdx = current.y * WORLD_ROWS + current.x;
        openSet[bestIdx] = openSet[--openCount];

        if (current.x == goal.x && current.y == goal.y) {
            ast_Node pathBuff[AST_MAX_NODES];
            int pathLen = 0;
            ast_Node step = goal;

            while (!(step.x == -1 && step.y == -1)) {
                pathBuff[pathLen++] = step;
                int idx = step.y * WORLD_ROWS + step.x;
                step = nodes[idx].parent;
            }

            int count = (pathLen < maxpath) ? pathLen : maxpath;
            for (int i = 0; i < count; i++) {
                outpath[i] = pathBuff[pathLen - 1 - i];
            }
            return count;
        }

        closed[current.y][current.x] = true;
        for (int d = 0; d < 8; d++) {
            int nx = current.x + ast_dirs[d][0];
            int ny = current.y + ast_dirs[d][1];
            if (!in_bounds(nx, ny)) continue;
            //TODO - implement tile walkable check here
            if (closed[ny][nx]) continue;

            ast_Node neighbour = {nx, ny};
            int nIdx = ny * WORLD_ROWS + nx;
            //int moveCost = grid[ny][nx].cost;; add move cost here
            int moveCost = 1;
            int tentativeG = nodes[currentIdx].g + moveCost;

            if (!nodes[nIdx].used || tentativeG < nodes[nIdx].g) {
                nodes[nIdx].used = true;
                nodes[nIdx].g = tentativeG;
                nodes[nIdx].h = ast_heuristic(neighbour, goal);
                nodes[nIdx].f = nodes[nIdx].g + nodes[nIdx].h;
                nodes[nIdx].parent = current;

                bool inOpen = false;
                for (int i = 0; i < openCount; i++) {
                    if (openSet[i].x == nx && openSet[i].y == ny) {
                        inOpen = true;
                        break;
                    }
                }
                if (!inOpen) openSet[openCount++] = neighbour;
            }
        }
    }
    return 0;
}


#endif