/** @file
 * Implementacja "batch mode" i funkcje pomocnicze
 *
 * @author Daniel Ciołek
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "gamma.h"
#include <errno.h>
#include "utilities.h"

/** @brief Sprawdza czy @p arg po zamienieniu z stringa na liczbę całkowitą
 * nie przekracza max uint32_t.
 * @param[in] arg   – wskiaźnik na strukturę przechowującą stan gry,
 *                          o rozmiarze większym lub równym 3.
 *                          różny od NULL.
 * @return true - @p arg nie przekracza zakresu uint32_t.
 *         false - @p arg przekracza zakres uint32_t.
 */

bool GoodArgument(char * arg) {
    char *end;

    uint64_t test = strtol(arg,&end,10);
    if(errno == ERANGE) {
        // sprawdza czy parametr nie jest większy od max uint64_t.
        return false;
    }
    if(test > 4294967295) {
        // sprawdza czy parametr nie jest większy od max uint32_t.

        return false;
    }

    return true;

}

/** @brief Sprawdza czy wejście z stdin jest poprawne dla komendy wykonującej
 * ruch oraz zapisuje kolejne argumenty na kolejne indeksy {0,1,2} @p input.
 * Jeśli argumenty były niepoprawne funkcja zwraca false ale niektóre z
 * argumentów mogą zostać zapisane do @p input.
 * @param[in,out] input   – wskiaźnik na strukturę przechowującą stan gry,
 *                          o rozmiarze większym lub równym 3.
 *                          różny od NULL.
 * @param[in] avoid       - tablica zawierająca znaki które maja być pomijane
 *                          przez strtok.
 * @return true - podane parametry były poprawne.
 *         false - podane parametry były niepoprawne.
 */

bool GetInputForMove(uint32_t **input,char *avoid) {
    char *buffor = NULL;
    int iter = 0;
    char *end;

    buffor = strtok(NULL, avoid);

    while (buffor != NULL) {
        if (iter > 2) {
            return false;
        }

        for (unsigned long i = 0; i < strlen(buffor); i++) {
            if ((int) buffor[i] < (int) '0' || (int) buffor[i] > (int) '9') {
                return false;
            }
        }

        if (GoodArgument(buffor)) {
            (*input)[iter] = (uint32_t) strtol(buffor, &end, 10);
        } else {
            return false;
        }

        iter++;
        buffor = strtok(NULL, avoid);
    }

    if (iter < 3) {
        return false;
    }

    return true;

}


/** @brief Sprawdza czy wejście z stdin jest poprawne dla komendy wykonującej
 * sprawdzającej parametry gracza oraz zapisuje kolejne argumenty na
 * kolejne indeksy {0,1,2} @p input.
 * Jeśli argumenty były niepoprawne funkcja zwraca false ale niektóre z
 * argumentów mogą zostać zapisane do @p input.
 * @param[in,out] input   – wskiaźnik na strukturę przechowującą stan gry,
 *                          o rozmiarze większym lub równym 3.
 *                          różny od NULL.
 * @return true - podane parametry były poprawne.
 *         false - podane parametry były niepoprawne.
 */

bool GetInputPlayer(uint32_t *input) {
    char *buffor = NULL;
    char avoid[] = " \t\v\f\r";

    buffor = strtok(NULL, avoid);

    if(buffor == NULL) {
        return false;
    }

    for(unsigned long i = 0; i < strlen(buffor); i++) {
        if((int)buffor[i] < (int)'0' || (int)buffor[i] > (int)'9') {
            if(!(i == strlen(buffor) - 1 && buffor[i] == '\n')) {
                return false;
            }
        }
    }
    char *end;

    if(GoodArgument(buffor)) {
        *input = (uint32_t) strtol(buffor,&end,10);;
    } else {
        return false;
    }

    buffor = strtok(NULL, avoid);

    if(buffor != NULL) {
        return false;
    }

    return true;

}


void BatchMode(gamma_t *g, int line_number) {

    // zmienna do wczytywania linii z getline
    // jest zaalokowana pesymistycznie ponieważ poprawne wejście nigdy nie
    // przekroczy 50 znaków.
    char *newline = malloc(sizeof(char) * 50);
    char avoid[] = " \t\v\f\r\n";

    while (getline2(&newline)) {
        if(isWhiteChar(newline[0]) || strlen(newline) == 0 ||
                            newline[strlen(newline) - 1] != '\n') {
            fprintf(stderr,"ERROR %d\n", line_number);
            line_number++;
            continue;
        }
        if(newline[0] != '#' && newline[0] != '\n') {
            char *buffor = NULL;

            buffor = strtok(newline, avoid);

            if(strcmp(buffor,"m") == 0) {

                uint32_t *input = malloc(sizeof(uint32_t) * 3);

                if(GetInputForMove(&input,avoid)) {
                    if(gamma_move(g,input[0],input[1],input[2])) {
                        printf("1\n");
                    } else {
                        printf("0\n");
                    }

                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

                free(input);

            } else if(strcmp(buffor,"g") == 0) {
                uint32_t *input = malloc(sizeof(uint32_t) * 3);

                if(GetInputForMove(&input,avoid)) {
                    if(gamma_golden_move(g,input[0],input[1],input[2])) {
                        printf("1\n");
                    } else {
                        printf("0\n");
                    }

                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

                free(input);

            } else if(strcmp(buffor,"b") == 0) {
                uint32_t input;

                if(GetInputPlayer(&input)) {
                    printf("%lu\n",gamma_busy_fields(g,input));
                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

            } else if(strcmp(buffor,"f") == 0) {
                uint32_t input;

                if(GetInputPlayer(&input)) {
                    printf("%lu\n",gamma_free_fields(g,input));
                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

            } else if(strcmp(buffor,"q") == 0) {
                uint32_t input;

                if(GetInputPlayer(&input)) {
                    if(gamma_golden_possible(g,input)) {
                        printf("1\n");
                    } else {
                        printf("0\n");
                    }
                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

            } else if(strcmp(buffor,"p") == 0) {

                buffor = strtok(NULL, avoid);

                if(buffor == NULL) {
                    char *board = gamma_board(g);
                    if(board != NULL) {
                        printf("%s", board);
                    } else {
                        fprintf(stderr,"ERROR %d\n", line_number);
                    }
                    free(board);
                } else {
                    fprintf(stderr,"ERROR %d\n", line_number);
                }

            } else {
                fprintf(stderr,"ERROR %d\n", line_number);
            }

        }
        line_number++;
    }

    free(newline);
}


