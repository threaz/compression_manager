#ifndef COMPRESSION_H
#define COMPRESSION_H

/*
 * Ten moduł odpowiada za przejrzyste kierowanie operacjami, jakie mają zostać
 * wykonane na plikach(jeszcze nie archiwach!). Jego funkcje są odpowiedzialne
 * za wywołanie odpowiednich metod kompresji i dekompresji, w zależności od
 * podanych przez użytkownika preferencji.
 */

#include <stdlib.h>

#include "archive.h"
#include "rle.h"
#include "huffman.h"
#include "lz77.h"
#include "lz78.h"

// definiuje używane typy kompresji
enum comp {
   NONE, RLE, HUFF, LZ77, LZ78
};

/*
 * Prosta funkcja do kopiowania jednego pliku do drugiego. Wykorzystuje
 * do tego szybkie funkcje fread i fwrite.
 *
 * Parametry:
 *    inFile: plik wejściowy, do skopiowania
 *    outFile: plik wyjściowy, do którego zostaną skopiowane dane z inFile
 *
 * Zwraca:
 *    ilość skopiowanych bajtów
 */
int copy_files(FILE *inFile, FILE *outFile);

/*
 * W zależności od podanej wartości w params, wykonuje kompresowanie odpowiednim
 * algorytmem na pliku wejściowym, zapisując wynik w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik, do którego zostaną skopiowane skompresowane dane
 *             (szczególnym przypadkiem kompresji jest jej brak)
 *
 * Zwraca:
 *     0: sukces
 *    -1: błąd spowodowany błędną wart. parametru/ów
 */
int compress_with_params(FILE *inFile, FILE *outFile, archive_data *params);

/*
 * W zależności od podanej wartości w params, wykonuje dekompresowanie odpowiednim
 * algorytmem na pliku wejściowym, zapisując wynik w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik, do którego zostaną skopiowane zdekompresowane dane
 *             (szczególnym przypadkiem kompresji jest jej brak)
 *
 * Zwraca:
 *     0: sukces
 *    -1: błąd spowodowany błędną wart. parametru/ów
 */
int decompress_with_params(FILE *inFile, FILE *outFile, archive_data *params);

#endif // COMPRESSION_H
