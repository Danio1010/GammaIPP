/** @file
 * Interfejs "batch_mode".
 *
 * @author Daniel Ciołek
 */

#include "gamma_structures.h"

#ifndef GAMMA_BATCH_MODE_H
#define GAMMA_BATCH_MODE_H


/** @brief Uruchamia gre w trybie "batch mode".
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 *                      róźny od NULL.
 * @param[in] line_number - numer wiersza, liczba dodatnia.
 */

void BatchMode(gamma_t *g, int line_number);

#endif //GAMMA_BATCH_MODE_H
