/** @file
 * Program pozwalający przeprowadzić rozgrywkę
 * w dwóch trybach wsadowym i interaktywnym.
 *
 * @author Daniel Ciołek
 */

#define  _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include "gamma.h"
#include "batch_mode.h"
#include "interactive_mode.h"
#include "errno.h"
#include "utilities.h"

/** @brief Sprawdza czy parametry podane na stdin są poprawne do
 * stworzenia nowej gry oraz zapisuje kolejne argumenty
 * na kolejne indeksy {0,1,2,3} @p input.
 * Jeśli argumenty były niepoprawne funkcja zwraca false ale niektóre z
 * argumentów mogą zostać zapisane do @p input.
 * @param[in,out] input   – wskaźnik na tablicę liczb całkowitych
 *                          o rozmiarze większym lub równym 4.
 *                          różny od NULL.
 * @return  true - podane parametry były poprawne.
 *          false - parametry nie były poprawne.
 */

bool GetInput(uint32_t **input) {
    char *buffor = NULL;
    char avoid[] = " \t\v\f\r";

    buffor = strtok(NULL, avoid);

    int iter = 0;

    while(buffor != NULL) {
        char *end = NULL;
        if(iter > 3) {
            return false;
        }

        if(strlen(buffor) == 0) {
            return false;
        }
        if(buffor[0] == '0') {
            return false;
        }

        for (unsigned long i = 0; i < strlen(buffor); i++) {
            if((int)buffor[i] < (int)'0' || (int)buffor[i] > (int)'9') {
                if(!(iter == 3 && i == strlen(buffor) - 1)) {
                    return false;
                }
            }
        }

        uint64_t test = strtol(buffor,&end,10);
        if(errno == ERANGE) {
            return false;
        }
        if(test > 4294967295) {
            return false;
        }

        (*input)[iter] = (uint32_t)test;
        if(errno == ERANGE) {
            return false;
        }
        iter++;

        buffor = strtok(NULL,avoid);

    }
    if(iter < 3) {
        return false;
    }

    return true;
}

/** @brief Sprawdza szerokość terminala.
 *  @return szerokość terminala.
 */
uint32_t GetTerminalWidth() {
    struct winsize size;
    ioctl( 0, TIOCGWINSZ, (char *) &size );
    return size.ws_col;
}

/** @brief Sprawdza wysokość terminala.
 *  @return wysokość terminala.
 */
uint32_t GetTerminalHeight() {
    struct winsize size;
    ioctl( 0, TIOCGWINSZ, (char *) &size );
    return size.ws_row;
}

/** @brief Sprawdza czy gra która stworzyliśmy w trybie Interaktywnym
 *  zmieści się w terminalu.
 *  @return true - Jeśli się zmieści.
 *         false - Jeśli się nie zmieści.
 */
bool WillItFit(uint32_t width, uint32_t height,uint32_t players,gamma_t *g) {
    uint32_t terminalHeight = GetTerminalHeight();
    uint32_t terminalWidth = GetTerminalWidth();
    uint32_t playerWidth = NumberOfDigits(g->number_of_players);
    if(NumberOfDigits(players) > 1) {
        if (terminalHeight >= height + 3 &&
            terminalWidth >= width * (playerWidth + 1) + 1 &&
            terminalWidth >= 2 * (uint32_t)NumberOfDigits(g->width * g->height)
                                + playerWidth + 12 &&
            terminalWidth >= 28 + (uint32_t)NumberOfDigits(g->max_areas)
                            + (uint32_t)NumberOfDigits(g->width * g->height)){

            return true;
        } else {
            return false;
        }
    } else {
        if (terminalHeight >= height + 3 &&
            terminalWidth >= width + 1 &&
            terminalWidth >= 2 * (uint32_t)NumberOfDigits(g->width * g->height)
                                + playerWidth + 12) {
            return true;
        } else {
            return false;
        }
    }
}

/** @brief Główna funkcja programu, zbiera wejście z stdin i jeśli parametry
 * wejściowe są poprawne i pozwalają (pamięciowo) na stworzenie nowej gry gamma
 * to wywołuje grę w "batch mode" lub "interactive mode".
 * w innym wypadku wypisuje na stderr "ERROR \a numer a\ wiersza".
 * @return 0.
 */
int main() {
    // zmienna do wczytywania linii z getline
    // jest zaalokowana pesymistycznie ponieważ poprawne wejście nigdy nie
    // przekroczy 50 znaków.
    char *newline = malloc(sizeof(char) * 50);
    char avoid[] = " \t\v\f\r\n";
    int line_number = 1;


    while (getline2(&newline)) {

        if(newline == NULL || strlen(newline) == 0) {
            // warunek brzegowy
            fprintf(stderr,"ERROR %d\n", line_number);
            line_number++;
            continue;
        }
        if(newline[strlen(newline) - 1] != '\n' || isWhiteChar(newline[0])) {
            // Warunek brzegowy - komenda nie kończąca sie enterem lub
            // posiadająca biały znak na początku komendy.
            fprintf(stderr,"ERROR %d\n", line_number);
            line_number++;
            continue;
        }

        char *buffor = NULL;

        buffor = strtok(newline, avoid);

        if (!(newline[0] == '#' || newline[0] == '\n' || buffor == NULL)) {
            // ignorowanie komentarzy i pustych komend

            if (strcmp("B", buffor) == 0) {

                uint32_t *input = malloc(sizeof(uint32_t) * 4);

                if (GetInput(&input)) {
                    gamma_t *new_game = gamma_new(input[0], input[1],
                                                  input[2], input[3]);

                    if(new_game != NULL) {
                        printf("OK %d\n",line_number);

                        line_number++;

                        BatchMode(new_game, line_number);

                        gamma_delete(new_game);
                        free(input);
                        //free(buffor);
                        free(newline);

                        return 0;

                    } else {
                        fprintf(stderr,"ERROR %d\n", line_number);
                    }
                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }
                free(input);
            } else {
                if (strcmp("I", buffor) == 0) {

                    uint32_t *input = malloc(sizeof(uint32_t) * 4);

                    if (GetInput(&input)) {
                        gamma_t *new_game = gamma_new(input[0], input[1],
                                                      input[2], input[3]);

                        if(new_game != NULL) {
                            if(!WillItFit(input[0],input[1],
                                        input[2] ,new_game)) {
                                printf("Prosze powiększyć okno terminala"
                                       " i sprobować ponownie.\n");
                            } else {

                                InteractiveMode(new_game);

                                gamma_delete(new_game);
                                free(input);
                                //free(buffor);
                                free(newline);

                                return 0;
                            }

                        } else {
                            fprintf(stderr,"ERROR %d\n", line_number);
                        }
                    } else {
                        fprintf(stderr,"ERROR %d\n", line_number);
                    }
                    free(input);

                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }
            }
        }

        line_number++;
    }


    free(newline);

    return 0;
}


