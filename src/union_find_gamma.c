/** @file
 * Implementacja Union-Find
 *
 * @author Daniel Ciołek
 */

#include <stdint.h>
#include "union_find_gamma.h"
#include "gamma_structures.h"

void Root(gamma_t *g, uint32_t *x, uint32_t *y) {

    if(g->board[*x][*y].parent_x != *x || g->board[*x][*y].parent_y != *y) {

        uint32_t temp_x = g->board[*x][*y].parent_x;
        uint32_t temp_y = g->board[*x][*y].parent_y;
        Root(g,&temp_x,&temp_y);
        g->board[*x][*y].parent_x = temp_x;
        g->board[*x][*y].parent_y = temp_y;
        *x = temp_x;
        *y = temp_y;
    } else {
        *x = g->board[*x][*y].parent_x;
        *y = g->board[*x][*y].parent_y;
    }
}

void Union(gamma_t *g, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {

    Root(g,&x1,&y1);
    Root(g,&x2,&y2);
    if(g->board[x1][y1].size < g->board[x2][y2].size) {
        g->board[x1][y1].parent_x = x2;
        g->board[x1][y1].parent_y = y2;

    } else {
        g->board[x2][y2].parent_x = x1;
        g->board[x2][y2].parent_y = y1;
    }
    if(g->board[x1][y1].size == g->board[x2][y2].size) {
        g->board[x1][y1].size++;
    }

}

void UnionAdjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t i = x;
    uint32_t j = y;

    if(!(x < g->width && y < g->height)) {
        return;
    }

    if(i > 0 && i - 1 < g->width) {
        if(g->board[i - 1][y].player_number == player) {
            Union(g,x,y,i - 1,j);
        }
    }
    i ++;
    if(i < g->width) {
        if(g->board[i][j].player_number == player) {
            Union(g,x,y,i,j);
        }
    }
    i = x;
    if(j > 0 && j - 1 < g->height) {
        if(g->board[i][j - 1].player_number == player) {
            Union(g,x,y,i,j - 1);
        }
    }
    j ++;
    if(j < g->height) {
        if(g->board[i][j].player_number == player) {
            Union(g,x,y,i,j);
        }
    }

}

void RemoveUnionOnAdjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(!(x <g->width && y < g->height)) {
        return;
    }

    uint32_t i = x;
    uint32_t j = y;

    if(g->board[x][y].visited == true || g->board[x][y].player_number != player) {
        return;
    }

    g->board[x][y].parent_x = x;
    g->board[x][y].parent_y = y;
    g->board[x][y].visited = true;
    g->board[x][y].size = 1;

    if(i > 0 && i - 1 < g->width) {
        if(g->board[i - 1][j].player_number == player) {
            RemoveUnionOnAdjacent(g,player,i - 1,j);
        }
    }
    i++;
    if(i < g->width) {
        if(g->board[i][j].player_number == player) {
            RemoveUnionOnAdjacent(g,player,i,j);
        }
    }
    i = x;
    if(j > 0 && j - 1 < g->height) {
        if(g->board[i][j - 1].player_number == player) {
            RemoveUnionOnAdjacent(g,player,i,j - 1);
        }
    }
    j++;
    if(j < g->height) {
        if(g->board[i][j].player_number == player) {
            RemoveUnionOnAdjacent(g,player,i,j);
        }
    }

}

void UnionAllAdjacent(gamma_t *g, uint32_t player,
                      uint32_t x, uint32_t y,uint32_t xp, uint32_t yp) {

    if(!(x <g->width && y < g->height)) {
        return;
    }

    if(g->board[x][y].player_number == player
       && !(g->board[x][y].visited)) {

        g->board[x][y].visited = true;
        Union(g,x,y,xp,yp);

        if(x > 0) {
            UnionAllAdjacent(g, player, x - 1, y, xp, yp);
        }
        if(x < g->width) {
            UnionAllAdjacent(g, player, x + 1, y, xp, yp);
        }
        if(y > 0) {
            UnionAllAdjacent(g, player, x, y - 1, xp, yp);
        }
        if(y < g->width) {
            UnionAllAdjacent(g, player, x, y + 1, xp, yp);
        }
    }
}

int NumOfUniqueAreas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t i = x;
    uint32_t j = y;
    long possible_areas[4][2];

    for(int z = 0; z < 4; z++) {
        for(int k = 0; k < 2; k++) {
            possible_areas[z][k] = -1;
        }
    }

    if(i > 0 && i - 1 < g->width) {
        if(g->board[i - 1][j].player_number == player) {
            uint32_t temp_x = i - 1;
            uint32_t temp_y = j;
            Root(g,&temp_x,&temp_y);

            possible_areas[0][0] = (long)temp_x;
            possible_areas[0][1] = (long)temp_y;
        }
    }
    i++;
    if(i < g->width) {
        if(g->board[i][j].player_number == player) {
            uint32_t temp_x = i;
            uint32_t temp_y = j;
            Root(g,&temp_x,&temp_y);

            possible_areas[1][0] = (long)temp_x;
            possible_areas[1][1] = (long)temp_y;
        }
    }
    i = x;
    if(j > 0 && j - 1 < g->height) {
        if(g->board[i][j - 1].player_number == player) {
            uint32_t temp_x = i;
            uint32_t temp_y = j - 1;
            Root(g,&temp_x,&temp_y);

            possible_areas[2][0] = (long)temp_x;
            possible_areas[2][1] = (long)temp_y;
        }
    }
    j++;
    if(j < g->height) {
        if(g->board[i][j].player_number == player) {
            uint32_t temp_x = i;
            uint32_t temp_y = j;
            Root(g,&temp_x,&temp_y);

            possible_areas[3][0] = (long)temp_x;
            possible_areas[3][1] = (long)temp_y;
        }
    }

    int res = 0;

    for (int k = 0; k < 4; k++) {
        int z = 0;
        if(possible_areas[k][0] != -1) {
            for (z = 0; z < k; z++) {
                if (possible_areas[z][0] != -1
                    && possible_areas[k][0] == possible_areas[z][0]
                    && possible_areas[k][1] == possible_areas[z][1])
                    break;
            }
            if (z == k)
                res++;
        }
    }

    return res;
}
