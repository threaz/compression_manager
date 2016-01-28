#ifndef LZ78_H
#define LZ78_H

/*
 * W module zaimplementowane zostały funkcje pozwalające na kompresję plików
 * przy użyciu algorytmu LZ78/LZW.
 *
 * Jego działanie opiera się na bardzo podobnej zasadzie do algorytmu opisanego
 * w module lz77.h. Wykorzystuje słownik, jednak nie jest on dynamicznym "okienkiem",
 * które przemieszcza się nad tekstem, ale statycznym wektorem, do którego zostają
 * dodawane nowe wpisy. Kiedy słownik ulegnie przepełnieniu, jest po prostu zerowany.
 *
 * Na początku wypełniamy początkowe wpisy (0-EOF_CODE) znakami o takich samych kodach
 * jak indeksy. Będą one symbolizowały pojedyncze znaki które występują w tekście.
 * Następnie szukamy słowa, które różni się o jeden znak ze słowem, które jest aktualnie
 * w słowniku. Umieszczamy je tam także. Usuwamy ostatni znak i kodujemy w pliku wejściowym
 * pozycję słowa w słowniku. Operacje na strumieniach bitowych przeprowadzamy za pomocą
 * biblioteki BitFile.
 *
 * Dekodowanie odbywa się w analogiczny sposób, a słownik ma dostatecznie dużo danych żeby
 * się odbudować. Jedyną sytuacją, kiedy słownik otrzymuje adres zakodowanego w słowniku
 * wyrazu, a nie ma go tam jeszcze, jest rozwiązywana na podstawie obserwacji, że szukanym
 * słowem jest aktualne słowo z dodanym na koniec pierwszym jego znakiem.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "bitfile.h"

#define MAX_DICT_S 4095      // maksymalny rozmiar słownika
#define CODE_WORD_S 12       // wielkość kodowanego słowa
#define CHAR_ENTRY UINT_MAX  // wartość, która charakteryzuje początkowe
                             // 0-256 znaków w słowniku
#define EOF_CODE 256         // przeznaczmy jeden znak na znacznik końca pliku



// Wygodna struktura do reprezentowania wpisów w słowniku
typedef struct
{
   unsigned int codeWord;   // kodowane słowo
   unsigned int prefixWord; // prefiks słowa, dla pierwszych 0-256 znaków jest to
                            // łatwo rozróżnialna wartość UINT_MAX
} entry_t;

// Struktura służąca do reprezentacji słownika wyrazów
typedef struct
{
   entry_t entry[MAX_DICT_S]; // właściwa tablica wpisów
   unsigned int pos;          // pozycja do wstawienia kolejnego słowa, jest to
                              // jednocześnie dobry licznik elementów słownika
} dict_t;

/*
 * Inicjalizuje słownik wartościami poczatkowymi.
 *
 * Zwraca:
 *    wskaźnik na nowo utworzony słownik
 */
dict_t *init_dictionary();

/*
 * Tworzy wpis, na podstawie zadanego prefiksu i słowa.
 *
 * Parametry:
 *    pref: prefiks wpisu
 *    word: słowo
 *
 * Zwraca:
 *    utworzony wpis
 */
entry_t make_entry(unsigned pref, unsigned word);

/*
 * Dodaje wpis do słownika.
 *
 * Parametry:
 *    ent: wpis
 *    dict: słownik
 */
void add_entry_to_dictionary(entry_t ent, dict_t *dict);

/*
 * Zwraca indeks wpisu ent w słowniku.
 *
 * Parametry:
 *    ent: wpis
 *    dict: słownik
 *
 * Zwraca:
 *    indeks znalezionego wpisu(>0)
 *    wartośc mniejszą od zera w p.p.
 */
int get_entry_index(entry_t ent, dict_t *dict);

/*
 * Zwraca wpis ze słownika o podanym jako argumend indeksie.
 *
 * Parametry:
 *    index: indeks wpisu
 *    dict: słownik
 *
 * Zwraca:
 *    wartość znalezionego wpisu
 */
entry_t get_entry(int index, dict_t *dict);

/*
 * Czyści słownik. Pozycja kolejnego wczytanego do słownika wyrazu będzie
 * wynosić dokładnie EOF_CODE+1, ponieważ na pierwszych 0-EOF_CODE miejscach
 * będą pojedyncze znaki, które rozpoczynają słownik.
 *
 * Parametry:
 *    dict: słownik do wyczyszczenia
 */
void clear_dictionary(dict_t *dict);

/*
 * Sprawdza, czy słownik jest pełny.
 *
 * Parametry:
 *    dict: słownik
 *
 * Zwraca:
 *    true: słownik pełny
 *    false: w p.p.
 */
bool is_full(dict_t *dict);

/*
 * Sprawdza, czy w słowniku znajduje się dany wpis.
 *
 * Parametry:
 *    ent: szukany wpis
 *    dict: słownik
 *
 * Zwraca:
 *    true: w słowniku znajduje się wpis
 *    false: w p.p.
 */
bool is_entry(entry_t ent, dict_t *dict);

/*
 * Sprawdza, czy w słowniku znajduje się wpis o danym indeksie.
 *
 * Parametry:
 *    index: indeks szukanego wpisu
 *    dict: słownik
 *
 * Zwraca:
 *    true: w słowniku znajduje się wpis o podanym indeksie
 *    false: w p.p.
 */
bool is_entry_index(int index, dict_t *dict);

/*
 * Rekurencyjnie przeszukuje wpisy w słowniku w poszukiwaniu pierwszego znaku
 * zadanego wpisu. Odwołujemy się ciągle do prefiksu analizowanego słowa,
 * jeżeli nie doszliśmy jeszcze do pierwszego znaku słowa. Jeżeli doszliśmy, zwracamy go.
 *
 * Parametry:
 *    ent: wpis, którego pierwszej litery szukamy
 *    dict: słownik, który przeszukujemy
 *
 * Zwraca:
 *    pierwszy znak wpisu
 */
int get_first_char(entry_t ent, dict_t *dict);

/*
 * Koduje wpis ze słownika, używając do tego skojarzonego z plikiem wyjściowym
 * strumienia bitowego, implementowanego przez bibliotekę BitFile.
 *
 * Parametry:
 *    ent: wpis, który chcemy zakodować
 *    dict: słownik, w kórym znajduje się wpis
 *    bfp: strumień bitów
 */
void encode_entry(entry_t ent, dict_t *dict, bit_file_t *bfp);

/*
 * Dekoduje wpis ze słownika, wynik zapisuje w pliku wyjściowym.
 *
 * Parametry:
 *    ent: wpis, który chcemy zdekodować
 *    dict: słownik, w kórym znajduje się wpis
 *    outFile: plik wyjściowy
 */
void decode_entry(entry_t ent, dict_t *dict, FILE *outFile);

/*
 * Czyta ze skojarzonego strumienia bitowego słowo o długości
 * CODE_WORD_S, wykorzystując do tego implementację biblioteki
 * BitFile.
 *
 * Parametry:
 *    bfp: strumień bitów, skojarzony z plikiem wejściowym
 *
 * Zwraca:
 *    przeczytane słowo
 */
int get_codeword(bit_file_t *bfp);

/*
 * Kompresuje plik wejściowy za pomocą algorytmu lz78/lzw, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void lz78_encode(FILE *inFile, FILE *outFile);

/*
 * Kompresuje plik wejściowy za pomocą algorytmu lz78/lzw, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void lz78_decode(FILE *inFile, FILE *outFile);

#endif // LZ78_H
