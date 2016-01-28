#ifndef BITFILE_H
#define BITFILE_H

/*
 * Biblioteka dostarczająca funkcji umożliwiających wygodne
 * korzystanie ze strumienia bitów. Udostępnia przyjazny interfejs,
 * który pomaga w implementacji programu. Umożliwia pisanie i odczytywanie
 * pojedyńczych bitów z pliku oraz zadaniej ich liczby. Biblioteka używa
 * do tego celu specjalnego 8-bitowego bufora, gdyż fizycznie, nie istnieją
 * takie możliwości.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define UCHAR_BITS (sizeof(unsigned char)*8)    // liczba bitów w char bez znaku
#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) // sprawdź pos-ty bit w liczbie var

// pomocnicze wartości, które jednoznacznie identyfikują
// "kierunek" strumienia bitów
typedef enum {
   READ,
   WRITE
} BF_MODE;

/*
 * Najważniejsza struktura danych, która udostępnia wygodny typ -
 * - bit_file_t, który symuluje strumień bitów. Skojarzony jest z nim
 * otwarty plik oraz tryb działania.
 */
typedef struct
{
    FILE *fp;                // wskaźnik do skojarzonego pliku
    unsigned char bitBuffer; // kontener bitów
    unsigned char bitLeft;   // licznik bitów w kontenerze
    BF_MODE mode;            // tryb, w jakim został otwarty strumień
} bit_file_t;

/*
 * Tworzy nowy "strumień bitów", który może być następnie w prosty sposób
 * zarządzany przez użytkownika.
 *
 * Parametry:
 *    stream: wskaźnik do pliku, z którym skojarzony ma zostać nowy strumień
 *    mode: tryb, w którym strumień ma zostać otwarty
 *
 * Zwraca:
 *    wsk. do strumienia bitów: w przypadku powodzenia
 *    NULL: w przypadku wystapienia błędu lub, gdy podany wskaźnik do pliku
 *          faktycznie się do żadnego pliku nie odwołuje
 */
bit_file_t *BitFile_make_new(FILE *stream, BF_MODE mode);

/*
 * Pisze pojedynczy bit do strumienia bitowego.
 *
 * Parametry:
 *    bit: jeżeli ma wartość 0, do strumenia zostanie dodany bit 0, w p.p.
 *       dodany zostanie bit 1
 *    bfp: wskaźnik do strumienia bitów
 *
 * Zwraca:
 *     0: sukces
 *    -1: w p.p
 */
int BitFile_write_bit(int bit, bit_file_t *bfp);

/*
 * Odczytuje pojedynczy bit ze strumienia bitowego.
 *
 * Parametry:
 *    bfp: wskaźnik do strumienia bitów
 *
 * Zwraca:
 *     0: gdy odczytany bit miał wartość 0
 *     wart. dodatnia: w p.p.
 */
int BitFile_read_bit(bit_file_t *bfp);

/*
 * Zapisuje znak podany jako parametr do strumienia bitów. Jeżeli w strumieniu
 * zajdują się już jakieś bity, wykonywane są odpowienie operacje, żeby wypisać
 * zalegające oraz wpisać do niego nowe bity.
 *
 * Parametry:
 *    ch: znak, który ma zostać zapisany do strumienia bitów
 *    bfp: wskaźnik do strumienia bitów
 *
 * Zwraca:
 *     0: sukces
 *    -1: w p.p
 */
int BitFile_write_char(unsigned char ch, bit_file_t *bfp);

/*
 * Odczytuje znak ze strumienia bitów.
 *
 * Parametry:
 *    bfp: wskaźnik do strumienia bitów
 *
 * Zwraca:
 *    liczba z przedziału 0-255: odczytany znak
 *    EOF: w przypadku napotkania końca pliku
 *    -2: w przypadku wystąpienia błędu
 */
int BitFile_read_char(bit_file_t *bfp);

/*
 * Odczytuje n bitów ze strumienia bitów.
 *
 * Parametry:
 *    nbits: ilość bitów do odczytania
 *    bfp: wskaźnik do strumienia bitów
 *
 * Zwraca:
 *    liczba z przedziału 0-255: odczytany znak zrzutowany na unsigned char
 *    EOF: w przypadku napotkania końca pliku, a w buforze nie było żadnego znaku
 *    przypadek szczególny: kiedy w buforze znajduje się mniej znaków niż chcemy
 *    z niego pobrać, a kolejnym znakiem na wejściu jest EOF, zwrócone zostaną
 *    dotychczasowo znajdujące się w buforze dane.
 */
int BitFile_read_nbits(int nbits, bit_file_t *bfp);

/*
 * Funkcja wypisująca aktualnie znajdujące się w strumieniu dane.
 * Po jej wywołaniu strumień powinien w stanie, jak po inicjalizacji.
 */
int BitFile_flush(bit_file_t *bfp);

#endif // BITFILE_H
