/** @file
 * Interfejs Union-Find i funkcji pokrewnych
 *
 * @author Daniel Ciołek
 */

#ifndef UNIONFINDGAMMA_H
#define UNIONFINDGAMMA_H

#include <stdbool.h>
#include <stdint.h>
#include "gamma_structures.h"

/** @brief Znajduje ojca (@p x,@p y) i zapisuje jego współrzędne w @p x i @p y
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in,out] x   – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in,out] y   – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void Root(gamma_t *g, uint32_t *x, uint32_t *y);

/** @brief Łączy obszar zawierający (@p x1,@p y1) z
 * obszarem zawierającym (@p x2,@p y2)
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x1       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y1       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] x2      – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y2     – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void Union(gamma_t *g, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

/** @brief Łączy element (@p x,@p y) w obszar z elementami sąsiadującymi
 * za pomocą Union-Find.
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void UnionAdjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Rozbija obszar zawierający w sobie element (@p x,@p y) i należący do
 * gracza player na jednoelementowe obszary (czyli usuwa unie w Union-Find).
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void RemoveUnionOnAdjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Łączy element (@p x,@p y) i elementy z nim sąsiadujące
 * w jeden obszar z obszarem który zawiera elementem (xp,yp)
 * przy pomocy Union-Find.
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] xp      – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] yp      – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 */
void UnionAllAdjacent(gamma_t *g, uint32_t player,
                      uint32_t x, uint32_t y,uint32_t xp, uint32_t yp);

/** @brief Oblicza ilośc unikalnych obszarów sąsiadujących z (@p x,@p y).
 * @param[in,out] g   – wskiaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza którego pola połączymy w obszar,
 *                      liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @return Ilośc unikalnych obszarów sąsiadujących z (@p x,@p y).
 */
int NumOfUniqueAreas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

#endif /* UNIONFINDGAMMA_H */
