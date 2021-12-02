/** @file
 * Funkcje pomocnicze.
 *
 * @author Daniel Ciołek
 */

#ifndef GAMMA_UTILITIES_H
#define GAMMA_UTILITIES_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Sprawdza czy @p c jest "białym znakiem".
 * @param[in] c - znak który sprawdzamy.
 * @return true - @p c jest "białym znakiem"
 *         false - @p c nie jest "białym znakiem".
 */
bool isWhiteChar(char c);

/** @brief Kopiuje string @p s.
 * @param[in] s   – string.
 * @return nowy string o tej samej zawartości co @p s.
 *         Jeśli s to NULL zwraca NULL.
 */
char* strcopy(char *s);

/** @brief Wpisuje @p x do @p board cyfra po cyfrze.
 * @param[in] x         – szerokość planszy, liczba dodatnia,
 * @param[in,out] board – wysokość planszy, liczba dodatnia,
 * @param[in] num       – liczba cyfr @p x, liczba dodatnia,
 * @param[in] iter      – aktualny indeks od którego bedziemy
 *                        wpisywać @p x do @p board, liczba nieujemna
 */
void IntToString(int x, char *board, int num, int iter);

/** @brief Liczy liczbe cyfr liczby.
 * @param[in] x   – liczba dodatnia,
 * @return liczba cyfr liczby @p x.
 */
int NumberOfDigits(uint32_t x);

/** @brief Wczytuje dane do @p newline. Jeśli posiadają niedozwolone znaki
 * funkcja zapisuje pusty String do newline. Pamięć zarezerwowana na newline
 * powinna być większa lub równa 50. Jeśli wejśćie się skończy zwraca false.
 *  @param[in,out] newline - String na którego jest zarezerwowane więcej lub
 *                          równo 50 bitów pamięci.
 *  @return true - Jeśli jest więcej wejścia do wczytania.
 *         false - Jeśli wejście się skończyło.
 */
bool getline2(char **newline);

/** @brief Zwraca minimum z dwoch liczb.
 * @param[in] a   – liczba calkowita.
 * @param[in] b   – liczba calkowita,
 * @return Minimum z liczb @p a i @p b.
 */
int min(int a,int b);
#endif //GAMMA_UTILITIES_H
