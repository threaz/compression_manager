#ifndef RLE_H
#define RLE_H

/*
 * Moduł odpowiedzialny za kompresowanie plików metodą RLE(Run Length Encoding).
 * Bazuje na pomyśle, że powtarzające się sekwencje znaków w pliku można zastąpić
 * po prostu ich liczbą, oszczędzając tym samym cenne bajty pamięci. Metoda ta jest
 * najbardziej efektywna przy kompresowaniu plików podobnych do map bitowych,
 * gdzie często występują długie sekwencje powtarzających się znaków. W innych przypadkach
 * daje kiepskie rezultaty.
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/*
 * Kompresuje plik wejściowy za pomocą algorytmu RLE, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void rle_encode(FILE *inFile, FILE *outFile);

/*
 * Dekompresuje plik wejściowy za pomocą algorytmu RLE, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void rle_decode(FILE *inFile, FILE *outFile);

#endif // RLE_H
