/** @file
 * Implementacja silnika gry i funkcje pomocnicze
 *
 * @author Daniel Ciołek
 */

#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "gamma_structures.h"
#include "union_find_gamma.h"
#include "utilities.h"

gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {

    if(width == 0 || height == 0 || areas == 0 || players == 0) {
        return NULL;
    }

    gamma_t *new_game;
    new_game = malloc(sizeof(gamma_t));

    if(new_game == NULL)
        return NULL;

    new_game->number_of_players = players;
    new_game->max_areas = areas;
    new_game->free_fields_under_max_areas = width * height;
    new_game->board = (board_element **) malloc(sizeof(struct board_element*) * width);
    if(new_game->board == NULL) {
        free(new_game);
        return NULL;
    }

    for(uint32_t i = 0; i < width; i++) {
        new_game->board[i] = (board_element *) malloc(sizeof(struct board_element) * height);
        if(new_game->board[i] == NULL) {

            for(int j = i; j >= 0; j --) {
                free(new_game->board[j]);
            }
            free(new_game->board);
            free(new_game);

            return NULL;
        }
        for(uint32_t j = 0; j < height; j++) {

            (new_game->board[i][j]).player_number = 0;
            (new_game->board[i][j]).parent_x = i;
            (new_game->board[i][j]).parent_y = j;
            (new_game->board[i][j]).size = 1;
            (new_game->board[i][j]).visited = false;

        }
    }

    new_game->players = (one_player *)malloc(sizeof(one_player) * players);
    if(new_game->players == NULL) {
        for(uint32_t j = 0; j < width; j++) {
            free(new_game->board[j]);
        }
        free(new_game->board);
        free(new_game);
        return NULL;
    }

    for(uint32_t i = 0; i < players; i ++) {
        (new_game->players[i]).number_of_areas = 0;
        (new_game->players[i]).busy_fields = 0;
        (new_game->players[i]).gold_possible = true;
    }

    new_game->width = width;
    new_game-> height = height;

    return new_game;
}

void gamma_delete(gamma_t *g) {
    if(g == NULL) {
        return;
    }
    for(uint32_t i = 0; i < g->width; i++) {
        free(g->board[i]);
    }
    free(g->board);
    free(g->players);
    free(g);
}

/** @brief Oblicza ilość elementów należących do gracza @p player
 * które sąsiadują z (@p x,@p y).
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @return Ilośc elementów należących do gracza @p player
 * które sąsiadują z (@p x,@p y).
 */
int NumberOfAdjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t i = x;
    uint32_t j = y;

    int result = 0;

    if(i > 0 && i - 1 < g->width) {
        if(g->board[i - 1][y].player_number == player) {
            result++;
        }
    }
    i++;
    if(i < g->width) {
        if(g->board[i][j].player_number == player) {
            result++;
        }
    }
    i = x;
    if(j > 0 && j - 1 < g->height) {
        if(g->board[i][j - 1].player_number == player) {
            result++;
        }
    }
    j++;
    if(j < g->height) {
        if(g->board[i][j].player_number == player) {
            result++;
        }
    }

    return result;
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if(g == NULL) {
        return false;
    }
    if(x >= g->width || y >= g->height || player == 0)
        return false;
    if(g->board[x][y].player_number != 0 || g->board[x][y].player_number == player) {
        return false;
    }
    if(player > g->number_of_players) {
        return false;
    }
    if(NumOfUniqueAreas(g,player,x,y) == 0) {
        if(g->players[player - 1].number_of_areas + 1 > g->max_areas){
            return false;
        }
        g->board[x][y].player_number = player;
        g->players[player - 1].number_of_areas++;

        g->players[player - 1].busy_fields++;
        g->free_fields_under_max_areas--;

        return true;
    } else {

        int unique_areas = NumOfUniqueAreas(g,player,x,y);

        g->board[x][y].player_number = player;

        UnionAdjacent(g,player,x,y);

        g->board[x][y].player_number = player;

        g->players[player - 1].number_of_areas -= (unique_areas - 1);

        g->players[player - 1].busy_fields++;
        g->free_fields_under_max_areas--;

        return true;
    }
}

/** @brief Realokuje pamięć na tablice board jeśli jej przewidywany rozmiar
 * przekracza zaalokowane miejsce w pamięci.
 * @param[in,out] board       - wskaźnik na tablice,liczba nieujemna
 * @param[in,out] board_size  – aktualny rozmiar tablicy,liczba nieujemna
 * @param[in,out] board_size_in_memory – aktualny rozmiar tablicy w pamięci,
 *                                       liczba nieujemna
 * @return Wskaźnik na board lub NULL, gdy nie udało się zaalokować pamięci.
 */
char* BoardRealloc(char *board, size_t *board_size, size_t *board_size_in_memory) {
    if (board_size > board_size_in_memory) {
        board = realloc(board, sizeof(char) * (*board_size * 2));
        if (board == NULL) {
            return NULL;
        }
        *board_size_in_memory = *board_size * 2;
    }
    return board;
}

char *gamma_board(gamma_t *g) {
    if(g == NULL) {
        return NULL;
    }
    
    char *board;
    if( NumberOfDigits(g->number_of_players) > 1) {
        // alokuje pamięć pesymistycznie
        board = malloc(sizeof(char) * (g->width + 1) * g->height *
                        (2+ NumberOfDigits(g->number_of_players))+ 2);
    } else {
        board = malloc(sizeof(char) * (g->width + 1) * g->height + 2);
    }
    if (board == NULL) {
        return NULL;
    }
    size_t board_size = 0;
    size_t board_size_in_memory = (g->width + 1) * g->height + 1;
    size_t iter = 0;

    for (int i = g->height - 1; i >= 0; i--) {
        for (uint32_t j = 0; j < g->width; j++) {

            if (g->board[j][i].player_number == 0) {

                board = BoardRealloc(board,&board_size,&board_size_in_memory);
                if (board == NULL) {
                    return NULL;
                }

                board_size++;
                board[iter] = '.';
                iter++;
            } else {
                int number_of_digits = NumberOfDigits(g->board[j][i].player_number);
                board_size += number_of_digits + 2;

                board = BoardRealloc(board,&board_size,&board_size_in_memory);
                if (board == NULL) {
                    return NULL;
                }

                if (number_of_digits > 9) {
                    board[iter] = '|';
                    iter++;
                }
                IntToString(g->board[j][i].player_number, board, number_of_digits, iter);
                iter += number_of_digits;

                if (number_of_digits > 9) {
                    board[iter] = '|';
                    iter++;
                }
            }
            board_size++;

            board = BoardRealloc(board,&board_size,&board_size_in_memory);
            if (board == NULL) {
                return NULL;
            }
        }
        board[iter] = '\n';
        iter++;
    }

    board = BoardRealloc(board,&iter,&board_size_in_memory);
    if (board == NULL) {
        return NULL;
    }

    board[iter] = '\0';
    return board;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if(g == NULL) {
        return 0;
    }
    if(player > 0 && player <= g->number_of_players) {
        return g->players[player - 1].busy_fields;
    }
    return 0;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if(g == NULL) {
        return 0;
    }
    uint64_t result = 0;
    if(player == 0 || player > g->number_of_players) {
        return 0;
    }
    if(g->players[player - 1].number_of_areas < g->max_areas) {
        return g->free_fields_under_max_areas;
    } else {
        for (uint32_t i = 0; i < g->width; i++) {
            for (uint32_t j = 0; j < g->height; j++) {
                if (g->board[i][j].player_number == 0
                    && NumberOfAdjacent(g,player,i,j) > 0) {
                    result++;
                }
            }
        }
    }

    return result;
}



/** @brief Zmienia g->board[x][y].visited z true na false w obszarze gracza
 * player który zawiera w sobie element (@p x,@p y)
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void RemoveVisited(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t i = x;
    uint32_t j = y;

    if(!(x <g->width && y < g->height)) {
        return;
    }

    if(g->board[x][y].visited == false) {
        return;
    }

    g->board[x][y].visited = false;

    if(i > 0 && i - 1 < g->width) {
        if(g->board[i - 1][j].player_number == player) {
            RemoveVisited(g,player,i - 1,j);
        }
    }
    i++;
    if(i < g->width) {
        if(g->board[i][j].player_number == player) {
            RemoveVisited(g,player,i,j);        }
    }
    i = x;
    if(j > 0 && j - 1 < g->height) {
        if(g->board[i][j - 1].player_number == player) {
            RemoveVisited(g,player,i,j - 1);
        }
    }
    j++;
    if(j < g->height) {
        if(g->board[i][j].player_number == player) {
            RemoveVisited(g,player,i,j);
        }
    }
}


/** @brief Zmienia parametry pola planszy o współrzędnych (@p x,@p y)
 * na domyślne.
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 *                      róźny od NULL.
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void ResetBoardElement(gamma_t *g, uint32_t x, uint32_t y) {
    g->board[x][y].player_number = 0;
    g->board[x][y].parent_x = x;
    g->board[x][y].parent_y = y;
    g->board[x][y].size = 1;
}

/** @brief Łączy w obszary za pomocą Union-Find
 * elementy sąsiadujące z (@p x,@p y) i należące do gracza @p player
 * (bez elementu (@p x,@p y)).
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void UnionNewAreas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    UnionAllAdjacent(g,player,x - 1,y,x - 1,y);
    RemoveVisited(g,player,x - 1,y);

    UnionAllAdjacent(g,player,x + 1,y,x + 1,y);
    RemoveVisited(g,player,x + 1,y);

    UnionAllAdjacent(g,player,x,y - 1,x,y - 1);
    RemoveVisited(g,player,x,y - 1);

    UnionAllAdjacent(g,player,x,y + 1,x,y + 1);
    RemoveVisited(g,player,x,y + 1);
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    //Przypadki brzegowe
    if(g == NULL) {
        return false;
    }
    if(x >= g->width || y >= g->height || player == 0) {
        return false;
    }
    if(g->board[x][y].player_number == 0
       || g->board[x][y].player_number == player) {
        return false;
    }
    if(!(g->players[player - 1].gold_possible)) {
        return false;
    }
    if(NumberOfAdjacent(g,player,x,y) == 0
       && g->players[player - 1].number_of_areas == g->max_areas) {
        return false;
    }

    uint32_t old_player_num = g->board[x][y].player_number;


    //Rozbijamy obszary należące do old_player_num na pojedyńcze elementy
    RemoveUnionOnAdjacent(g,old_player_num,x,y);
    RemoveVisited(g,old_player_num,x,y);

    //Zmieniamy parametry miejsca (x,y) na planszy na podstawowe.
    //Co może podzielić istniejący obszar na wiele obszarów.
    ResetBoardElement(g,x,y);

    //Łączymy elementy w nowo powstałe obszary za pomocą Union-Find
    UnionNewAreas(g,old_player_num,x,y);

    int Num_of_unique_areas_after_gold = NumOfUniqueAreas(g,old_player_num,x,y);

    uint32_t New_number_of_areas =
            Num_of_unique_areas_after_gold - 1
            + g->players[old_player_num - 1].number_of_areas;


    //Jeśli podział obszaru jest nielegalny cofamy zmiany
    //po czym scalamy nowe obszary w jeden.
    if(New_number_of_areas > g->max_areas) {
        g->board[x][y].player_number = old_player_num;
        UnionAdjacent(g,old_player_num,x,y);
        return false;
    }

    g->players[old_player_num - 1].number_of_areas = New_number_of_areas;

    //Stawiamy pionek gracza o numerze player w miejscu (x,y)
    if(gamma_move(g,player,x,y)) {
        g->players[player - 1].gold_possible = false;

        g->players[old_player_num - 1].busy_fields--;
        g->free_fields_under_max_areas++;
        return true;
    } else {
        return false;
    }
}

/** @brief Sprawdza czy na danym polu @p x @p y grac @p player może wykonać
 * złoty ruch.
 *  @param[in,out] g - zmienna przechowujaca stan gry.
 *  @param[in] x - współrzędna x gracza @p player, liczba nieujemna, mniejsza
 *                 od g->width.
 *  @param[in] y - współrzędna y gracza @p player, liczba nieujemna, mniejsza
 *                 od g->height.
 *  @param[in] player - numer gracza liczba całkowita nieujemna, mniejsza od
 *                      g->number_of_players.
 *  @return true - Jeśli gracz @p player moze wykonac zloty ruch na to pole.
 *         false - Jeśli gracz @p player nie moze wykonac zlotego ruchu na
 *                 to pole.
 */
bool TestForGolden(gamma_t *g, uint32_t x, uint32_t y, uint32_t player) {

    if(NumberOfAdjacent(g,player,x,y) == 0
       && g->players[player - 1].number_of_areas == g->max_areas) {
        return false;
    }

    uint32_t old_player_num = g->board[x][y].player_number;

    //Rozbijamy obszary należące do old_player_num na pojedyńcze elementy
    RemoveUnionOnAdjacent(g,old_player_num,x,y);
    RemoveVisited(g,old_player_num,x,y);

    //Zmieniamy parametry miejsca (x,y) na planszy na podstawowe.
    //Co może podzielić istniejący obszar na wiele obszarów.
    ResetBoardElement(g,x,y);

    //Łączymy elementy w nowo powstałe obszary za pomocą Union-Find
    UnionNewAreas(g,old_player_num,x,y);

    int Num_of_unique_areas_after_gold = NumOfUniqueAreas(g,old_player_num,x,y);

    uint32_t New_number_of_areas =
            Num_of_unique_areas_after_gold - 1
            + g->players[old_player_num - 1].number_of_areas;


    //Jeśli podział obszaru jest nielegalny cofamy zmiany
    //po czym scalamy nowe obszary w jeden.
    if(New_number_of_areas > g->max_areas) {
        g->board[x][y].player_number = old_player_num;
        UnionAdjacent(g,old_player_num,x,y);
        return false;
    } else {
        g->board[x][y].player_number = old_player_num;
        UnionAdjacent(g,old_player_num,x,y);
        return true;
    }
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {


    if(g == NULL || player == 0 || player > g->number_of_players) {
        return false;
    }
    if(g->players[player - 1].gold_possible) {
        for(uint32_t i = 0; i < g->width; i++) {
            for(uint32_t j = 0; j < g->height; j++) {
                if(g->board[i][j].player_number != 0 && g->board[i][j].player_number != player) {
                    if(TestForGolden(g,i,j,player)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
