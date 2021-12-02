/** @file
 * Interfejs "interactive mode".
 *
 * @author Daniel Ciołek
 */

#include "gamma_structures.h"

#ifndef GAMMA_INTERACTIVE_MODE_H
#define GAMMA_INTERACTIVE_MODE_H

/** @brief Uruchamia gre w trybie "interactive mode".
 *  Dodano kolory graczy na planszy, kolory nie powtarzają się dla
 *  gry z iloscią graczy mniejszą lub rónwą 13.
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 *                      róźny od NULL.
 */
void InteractiveMode(gamma_t *g);

#endif //GAMMA_INTERACTIVE_MODE_H
