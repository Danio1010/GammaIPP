/** @file
 * Implementacja funkcji pomocniczych.
 *
 * @author Daniel Ciołek
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int min(int a,int b) {
    if(a > b) {
        return b;
    } else {
        return a;
    }
}

char* strcopy(char *s) { //zwraca wskaznik na nowy string identyczny z s
    if(s == NULL) {
        return NULL;
    }
    uint32_t len = strlen(s);
    char *new = NULL;
    new = realloc(new,sizeof(char)*(len + 1));
    // ochrona przed brakiem pamięci
    if(new == NULL) {
        exit(1);
    }

    for(uint32_t i = 0; i < len; i++) {
        new[i] = s[i];
    }

    new[len] = '\0';

    return new;
}

/** @brief Sprawdza czy @p c jest "białym znakiem".
 * @param[in] c - znak który sprawdzamy.
 * @return true - @p c jest "białym znakiem"
 *         false - @p c nie jest "białym znakiem".
 */
bool isWhiteChar(int c) {
    return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}

void IntToString(int x, char *board, int num, int iter) {
    char *temp = malloc(sizeof(char) * num + 1);
    int k = 0;
    while(x > 0) {
        temp[k] = (char)((x - (x/10)*10) + '0');
        x = x/10;
        k++;
    }

    temp[k] = '\0';

    for(int l = 0; l < num; l++) {
        board[iter + l] = temp[num - 1 - l];
    }
    free(temp);
}

uint64_t NumberOfDigits(uint32_t x) {
    int result = 0;
    while(x > 0) {
        x = x/10;
        result ++;
    }

    return result;
}

/** @brief Sprawdza czy @p c jest numerem w kodzie ASCII.
 * @param[in] c - liczba całkowita.
 * @return true - c to liczba w kodzie ASCII.
 *         false - c nie jest liczbą w kodzie ASCII.
 */
bool isNumber(int c) {
    if((int)c < (int)'0' || (int)c > '9') {
        return false;
    } else {
        return true;
    }
}

/** @brief Sprawdza czy @p c jest znakiem z zakresu od 'A' do 'z'
 *  w kodzie ASCII.
 * @param[in] c - liczba całkowita.
 * @return true - c jest znakiem z zakresu od 'A' do 'z' w kodzie ASCII.
 *         false - c nie jest znakiem z zakresu od 'A' do 'z' w kodzie ASCII.
 */
bool isAcceptable(int c) {
    if((int)c >= (int)'A' && (int)c <= (int)'z') {
        return true;
    } else {
        return false;
    }
}

bool getline2(char **newline) {
    int c = getchar();
    if(c == (int)'\n') {
        (*newline)[0] = '\n';
        (*newline)[1] = '\0';
        return true;
    }
    if(c == (int)'#') {
        c = getchar();
        while(c != '\n' && c != EOF) {
            c = getchar();
        }
        (*newline)[0] = '#';
        (*newline)[1] = '\n';
        (*newline)[2] = '\0';

        return true;
    }
    int iter = 0;
    int numberOfWhite = 0;
    while(c != '\n' && c != EOF) {

        while(isWhiteChar(c)) {
            if(numberOfWhite == 0) {
                if (iter > 46) {
                    while(c != '\n' && c != EOF) {
                        c = getchar();
                    }
                    (*newline)[0] = '\0';
                    return true;
                }
                numberOfWhite = 1;
                (*newline)[iter] = c;
                iter++;
            }
            c = getchar();
        }

        if(isNumber(c) || isAcceptable(c)) {
            if(iter > 46) {
                while(c != '\n' && c != EOF) {
                    c = getchar();
                }
                (*newline)[0] = '\0';
                return true;
            }
            numberOfWhite = 0;
            (*newline)[iter] = c;
            iter++;
        } else {
            if(c == '\n') {
                if(iter > 46) {
                    while(c != '\n' && c != EOF) {
                        c = getchar();
                    }
                    (*newline)[0] = '\0';
                    return true;
                }
                (*newline)[iter] = '\n';
                iter++;
                break;
            } else {
                while(c != '\n' && c != EOF) {
                    c = getchar();
                }
                (*newline)[0] = '\0';
                return true;
            }

        }

        c = getchar();
        if(c == '\n') {
            if(iter > 46) {
                while(c != '\n' && c != EOF) {
                    c = getchar();
                }
                (*newline)[0] = '\0';
                return true;
            }
            (*newline)[iter] = '\n';
            iter++;
        }
    }

    (*newline)[iter] = '\0';

    if(strlen(*newline) > 2 && (*newline)[strlen(*newline) - 1] == '\n' &&
       isWhiteChar((*newline)[strlen(*newline) - 2])) {
        (*newline)[strlen(*newline) - 2] = '\n';
        (*newline)[strlen(*newline) - 1] = '\0';
    }
    if(c == EOF) {
        return false;
    }
    return true;
}