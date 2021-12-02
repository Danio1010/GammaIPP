/** @file
 * Interfejs Union-Find i funkcji pokrewnych
 *
 * @author Daniel Ciołek
 */

#ifndef GAMMASTRUCTURES_H
#define GAMMASTRUCTURES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Struktura przechowująca parametry jednego pola planszy
 * oraz parametry potrzebne do zastosownia Union-Find:
 */
typedef struct board_element {
    uint32_t player_number; /**< numer gracza obecnego na polu */
    uint32_t parent_x;      /**< współrzędna x ojca pola */
    uint32_t parent_y;      /**< spółrzędna y ojca pola */
    uint64_t size;          /**< wartość size pola potrzebna do Union-Find */
    bool visited;           /**< zmienna potrzebna do bfs */
} board_element;

/**
 * @brief Struktura przechowująca stan pojedyńczego gracza.
 */
typedef struct one_player {

    uint32_t number_of_areas; /**< aktualna liczba obszarów */
    uint64_t busy_fields;     /**< ilość pól zajmowanych przez gracza */
    bool gold_possible;       /**< zmienna mówiąca czy gracz może wykonać
                                *  złoty ruch */

} one_player;

/**
 * @brief Struktura przechowująca stan gry.
 */
typedef struct gamma {

    board_element **board; /**< dwuwymiarowa tablica elementów planszy */

    uint64_t free_fields_under_max_areas;/**< zmienna do zliczania wolnych pól
                                             * dla graczy którzy mają mniej
                                             * obszarów niż maksymalna liczba
                                             * obszarów */

    one_player *players;                 /**< tablica stanów pojedyńczych graczy
                                            * gdzie na indeksie @p i znajdują
                                            * sie parametry @p i + 1 gracza */

    uint32_t width;                      /**< szerekość planszy */
    uint32_t height;                     /**< wysokość planszy */
    uint32_t number_of_players;          /**< ilość graczy */
    uint32_t max_areas;                  /**< maksymalna ilość obszarów */
} gamma_t;

#endif //GAMMASTRUCTURES_H
