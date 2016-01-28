#ifndef LZ77_H
#define LZ77_H

/*
 * W module zaimplementowane zostały funkcje pozwalające na kompresję plików
 * przy użyciu algorytmu LZ77/LZSS.
 *
 * Kompresja opiera się na przeszukiwaniu tekstu w poszukiwaniu powtarzających się ciągów znaków,
 * gdzie następnie zastępowane są one przez offset względem aktualnej pozycji w słowniku. Słownikiem
 * jest tzw. "sliding window", które przemieszcza się po tekście jak mała ramka. Do sprawdzania
 * aktualnie przetwarzanych znaków służy mniejszy bufor. Obydwa twory zostały zaimplementowane za
 * pomocą wygodnej tablicy cyklicznej. Jeżeli długość słowa nie przekracza minimum, to pierwszy znak
 * słowa zostanie zakodowany zwyczajnie, jaki bit UNCODED oraz znak. W p.p. kodujemy pierwszy bit
 * jako ENCODED. Dalej 12 bitów offsetu i 4 na długość słowa.
 *
 * Dzięki zainicjalizowaniu słownika na początku tym samym znakiem, co przy kompresji słownik
 * może zostać odbudowany. Kiedy podczas czytania napotykamy na bit UNCODED, to czytamy z wejścia
 * kolejny znak i go wypisujemy. Jeżeli ENCODED, czytamy kolejne 16 bitów i wypisujemy na wyjście
 * odpowiednią pozycję w słowniku o określonej długości.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bitfile.h"

#define ENCODED 1 // zakodowany znak
#define UNCODED 0 // niezakodowany

#define START_SIGN ' ' // znak początkowy
                       // używany w kompr. i dekompr.

#define OFFSET 12      // ilość bitów na przesunięcie w tekście
#define LENGTH  4      // ilość bitów na długość słowa

#define MAX_DICT (1 << OFFSET) // 2^(12)
#define MAX_UNCODED 2  // maksynalna liczba znaków, które nie zostaną
                       // zakodowane, ale jedynie przepisane
                       // (oszczędza pamięć)

// maksymalna długość bufora, bierze się stąd, że na LENGTH bitach możemy
// zapisać teoretycznie ciąg o długosći co najwyżej 2^(LENGTH) bitów
// jednak przyjmując konwencję, że 0 oznacza 3 znaki(bo dwóch w ogóle nie kodujemy)
// możemy zapisać w tym przypadku 15 + 3 = 18 znaki
#define MAX_BUF  ((1 << LENGTH) + MAX_UNCODED)

// Jeden typ danych, który trzyma jednocześnie offset i długość ciągu znaków
// znacznie ułatwia pracę z programem.
typedef struct
{
   unsigned int data_offset;
   unsigned int data_length;
} match_data_t;

/*
 * Przeszukuje zewnętrzny słownik w poszukiwaniu najdłuższego podsłowa, które
 * aktualnie znajduje się w buforze i w słowniku.
 *
 * Parametry:
 *    dict_pos: pozycja w słowniku
 *    buf_pos:  pozycja w buforze
 *
 * Zwraca:
 *    struktura, która opisuje jaki jest offset znalezionego słowa w słowniku
 *    od aktualnej pozycji oraz jaka jest długość jego długość
 */
match_data_t findPattern(unsigned int dict_pos, unsigned int buf_pos);

/*
 * Kompresuje plik wejściowy za pomocą algorytmu lz77/lzss, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void lz77_encode(FILE *inFile, FILE *outFile);

/*
 * Dekompresuje plik wejściowy za pomocą algorytmu lz77/lzss, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void lz77_decode(FILE *inFile, FILE *outFile);

#endif // LZ77_H
