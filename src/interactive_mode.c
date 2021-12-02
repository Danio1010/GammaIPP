/** @file
 * Implementacja trybu "interactive"
 *
 * @author Daniel Ciołek
 */

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "gamma_structures.h"
#include "gamma.h"
#include <stdio.h>
#include "utilities.h"

/** @brief Sprawdza czy jakikolwiek z graczy może wykonać ruch.
 * @param[in,out] g   – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                      różny od NULL.
 * @return  true - istnieje gracz który może wykonać ruch
 *          false - nie istnieje taki gracz.
 */
bool CanAnyPlayerMakeAMove(gamma_t *g) {
    for(uint32_t i = 1; i < g->number_of_players ; i++) {
        if(gamma_free_fields(g,i) > 0 || gamma_golden_possible(g,i)) {
            return true;
        }
    }

    return false;
}

/** @brief Sprawdza czy dany gracz może wykonać ruch.
 * @param[in,out] g   – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                      różny od NULL.
 * @param[in] player  - numer gracza, liczba dodatnia.
 * @return  true - gracz może wykonać ruch
 *          false - gracz nie może wykonać ruchu.
 */
bool CanPlayerMakeAMove(gamma_t *g, uint32_t player) {
    return (gamma_free_fields(g,player) > 0) || gamma_golden_possible(g,player);
}

/** @brief Zmienna przechowująca orginalny stan terminala
 */
static struct termios original_mode;

/** @brief Przywraca stan terminala do stanu przed wywołaniem trybu
 * interaktywnego.
 */
void PutTerminalInOriginalMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_mode);
}

/** @brief Wyłacza flagi ECHO i ICANON w ustawieniach terminala, oraz przy
 * użyciu atexit(PutTerminalInOriginalMode) upewnia że terminal powróci do
 * stanu przed uruchomieniem trybu interaktywnego. W przypadku niepowodzenia
 * w ustawianiu nowych opcji terminala program kończy sie kodem 1.
 */

void PutTerminalInInteractiveMode() {
    struct termios new_mode;
    atexit(PutTerminalInOriginalMode);
    tcgetattr(STDIN_FILENO, &original_mode);

    new_mode = original_mode;

    new_mode.c_lflag &= ~(ECHO | ICANON);

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_mode) == -1) {
        exit(1);
    }
}

/** @brief Wypisuje ' ' w miejsce statusu gracza.
 * @param[in,out] g   – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                      różny od NULL.
 */
void ClearPlayerStatus(gamma_t *g) {
    uint64_t number_to_clear = 2 * NumberOfDigits(g->width * g->height) + 12
                                + NumberOfDigits(g->number_of_players);
    for(uint64_t i = 0; i < number_to_clear; i++) {
        printf(" ");
    }
    printf("\n");
    uint64_t number_to_clear2 = 2 * (uint32_t)NumberOfDigits(g->max_areas) + 28;
    for(uint64_t i = 0; i < number_to_clear2; i++) {
        printf(" ");
    }
}

/** @brief Wypisuje statystyki gracza który aktualnie wykonuje ruch.
 * Czyli jego numer,ilość zajętych pól,ilość wolnych pól oraz G w przypadku
 * możliwośći wykonania złotego ruchu.
 * @param[in,out] g         – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                            różny od NULL.
 * @param[in] player_number - numer gracza którego statystyki wypisujemy, liczba
 *                            dodatnia mniejsza od @p g->number_of_players
 * @param[in] x             - współrzędna x gracza, liczba dodatnia.
 * @param[in] y             - współrzędna y gracza, liczba dodatnia.
 */
void WritePlayerStatus(gamma_t *g, uint32_t player_number, uint32_t x, uint32_t y) {
    char gold = ' ';
    if(gamma_golden_possible(g,player_number)) {
        gold = 'G';
    }

    printf("%c[%d;%df",0x1B,g->height + 1,1);

    ClearPlayerStatus(g);

    printf("%c[%d;%dH",0x1B,g->height + 1,1);

    printf("PLAYER %d %lu %lu %c\n",
            player_number,gamma_busy_fields(g,player_number),
            gamma_free_fields(g,player_number), gold);

    printf("Liczba obszarów gracza: %d na %d",
            g->players[player_number - 1].number_of_areas,
            g->max_areas);

    if(NumberOfDigits(g->number_of_players) > 1) {
        printf("\x1b[%d;%df", g->height - y,
               x * (NumberOfDigits(g->number_of_players) + 1) + 1);
    } else {
        printf("\x1b[%d;%df", g->height - y,
               x * (NumberOfDigits(g->number_of_players)) + 1);
    }
}

/** @brief Wypisuje startową pusta plansze do gry.
 * @param[in,out] g         – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                            różny od NULL.
 */
void RenderBoard(gamma_t *g) {
    uint32_t max_width = NumberOfDigits(g->number_of_players);
    if(max_width == 1) {
        max_width = 0;
    }

    for(uint32_t i = 0; i < g->height; i++) {
        for(uint32_t j = 0; j < g->width; j++) {
            printf(".");
            for(uint32_t k = 0; k < max_width; k++) {
                printf(" ");
            }
        }
        printf("\n");
    }

}
/** @brief Wypisuje końcowe statystki gracza.
 * Czyli jego numer oraz ilość pól zajętych w czasie gry.
 * @param[in,out] g         – wskaźnik na struktrurę przechowującą aktualny stan gry
 *                            różny od NULL.
 */
void WriteSummary(gamma_t *g) {
    for(uint32_t i = 1; i <= g->number_of_players; i++) {
        printf("PLAYER  %u  %lu\n", i, gamma_busy_fields(g,i));
        printf("Gracz ma %d na %d obszarow.\n",
                g->players[i-1].number_of_areas,g->max_areas);
    }
}

/** @brief Przypisuje każdemu z graczy kolor zaleznie od numeru gracza.
 *  Dla liczby graczy mniejszej niz 13 każdy gracz ma unikalny numer. Powyżej
 *  tej ilości graczy numery się powtarzają.
 * @param[in] playerNumber  – numer gracza.
 * @return Numer opisujący kolor w escape codes.
 */
int GiveColor(uint32_t playerNumber) {
    int colorTable[14] =
            {40,41,42,43,44,45,46,100,101,102,103,104,105,106};

    return colorTable[(playerNumber - 1) % 14];
}

/** @brief Sprawdza czy @p c jest połączony z postawieniem pionka lub złotym
 * ruchem. Jeśli tak to zwraca true oraz stawia pionek
 * w miejscu @p cursor_x @p cursor_y.
 * @param[in,out] g         – wskaźnik na struktrurę przechowującą aktualny
 *                            stan gry różny od NULL.
 * @param[in] c             - char zrzutowany na inta.
 * @param[in] player_number - numer gracza który wykonuje ruch, liczba dodatnia.
 * @param[in] cursor_x      - współrzędna x gracza, liczba nieujemna.
 * @param[in] cursor_y      - współrzędna y gracza, liczba nieujemna.
 * @param[in] width_of_player - szerokość numeru gracza, liczba dodatnia.
 */
bool CheckForMove(gamma_t *g, int c, uint32_t player_number, uint32_t cursor_x,
                uint32_t cursor_y, uint32_t width_of_player) {
    switch (c) {
        case 'G':
        case 'g': {
            if (gamma_golden_move(g, player_number, cursor_x, cursor_y)) {

                if(width_of_player > 0) {
                    for (uint32_t i = 1; i <= width_of_player; i++) {
                        printf(" ");
                    }

                    printf("%c[%dD", 0x1B, width_of_player);
                }
                printf("\x1b[%dm", GiveColor(player_number + 1));
                printf("%d", player_number);
                printf("\x1b[30m");
                printf("%c[%dD", 0x1B, NumberOfDigits(player_number));
                printf("\x1b[0m");
                return true;
            }
            break;
        }
        case ' ': {
            if (gamma_move(g, player_number, cursor_x, cursor_y)) {
                printf("\x1b[%dm", GiveColor(player_number + 1));
                for (uint32_t i = 1; i <= width_of_player + 1; i++) {
                    printf(" ");
                }
                printf("%c[%dD", 0x1B, width_of_player + 1);
                printf("%d", player_number);
                printf("\x1b[30m");
                printf("%c[%dD", 0x1B, NumberOfDigits(player_number));
                printf("\x1b[0m");

                return true;
            }
            break;
        }
        default: {
            return false;
        }
    }
    return false;
}

void InteractiveMode(gamma_t *g) {

    uint32_t width_of_player = NumberOfDigits(g->number_of_players);
    if(width_of_player == 1) {
        width_of_player = 0;
    }

    // zmienne do przechowywania pozycji gracza na planszy
    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;

    uint32_t player_number = 1;

    // zmienna informująca o zakończeniu gry
    bool exit = false;

    PutTerminalInInteractiveMode();

    // czyszczenie ekranu terminala
    printf("%c[2J",0x1B);
    // przesunięcie kursora w lewy górny róg
    printf("%c[H",0x1B);

    // wygenerowanie planszy
    RenderBoard(g);

    // przesunięcie kursora na pozycje (0,0) planszy
    printf("%c[A", 0x1B);

    while(CanAnyPlayerMakeAMove(g) && !exit) {
        for(player_number = 1;
            player_number <= g->number_of_players && !exit; player_number++) {

            if (CanPlayerMakeAMove(g, player_number)) {
                
                WritePlayerStatus(g,player_number,cursor_x,cursor_y);

                bool end_move = false;

                int c = (int)getchar();
                while (!exit && !end_move) {
                    switch (c) {
                        case (4): {
                            exit = true;
                            end_move = true;
                            break;
                        }
                        case 'C':
                        case 'c': {
                            end_move = true;
                            break;
                        }
                        case ('\033'): {
                            c = getchar();

                            while(c == '\033') {
                                c = getchar();
                            }
                            if(c != '[') {
                                continue;
                            }

                            int temp = getchar();

                            switch (temp) {
                                case 'A':
                                    if (cursor_y < g->height - 1) {
                                        printf("%c[A", 0x1B);
                                        cursor_y++;
                                    }
                                    break;
                                case 'B':
                                    if (cursor_y >= 1) {
                                        printf("%c[B", 0x1B);
                                        cursor_y--;
                                    }
                                    break;
                                case 'C':
                                    if (cursor_x < g->width - 1) {
                                        printf("%c[%dC", 0x1B,
                                                width_of_player + 1);
                                        cursor_x++;
                                    }
                                    break;
                                case 'D':
                                    if (cursor_x >= 1) {
                                        printf("%c[%dD", 0x1B,
                                                width_of_player + 1);
                                        cursor_x--;
                                    }
                                    break;

                                default: {
                                    end_move = CheckForMove(g,temp,
                                            player_number,cursor_x,
                                            cursor_y,width_of_player);
                                    break;
                                }
                            }

                            break;
                        }
                        default: {
                            end_move = CheckForMove(g,c,player_number,cursor_x,
                                                    cursor_y,width_of_player);
                            break;
                        }
                    }

                    if(!end_move) {
                        c = (int) getchar();
                    }
                }

            }
        }
    }

    // przesunięcie kursora poniżej planszy
    printf("%c[%d;%dH",0x1B,g->height + 1,1);

    ClearPlayerStatus(g);

    // przesunięcie kursora poniżej planszy
    printf("%c[%d;%dH",0x1B,g->height + 1,1);

    WriteSummary(g);

    PutTerminalInOriginalMode();
}


