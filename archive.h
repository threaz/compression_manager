#ifndef ARCHIVE_H
#define ARCHIVE_H

/*
 * Biblioteka definiuje typy oraz funkcje używane do pracy z archiwami.
 */

#include "rle.h"
#include <string.h>

#define MAX_NAME 100            // maksymalna długość nazwy pliku
#define MAX_FILES 100           // maksymalna liczba plików w archiwum

typedef struct {
   char *name_buf[MAX_NAME];    // bufor na nazwy plikow, które nie są archiwami
   unsigned int name_pos;       // aktualna pozycja w tym buforze
} name_buffer;

typedef struct {
   char ar_name_buf[MAX_NAME];  // przechowuje nazwę archiwum
   name_buffer  buf;            // bufor na nazwy plikow
   unsigned int nfiles;         // licznik plików
   unsigned int compression;    // rodzaj kompresji, domyślnie powinien być NONE
} archive_data;

// opisuje "kierunek" wykonania akcji na archiwum
enum direction { NOWHERE, COMPRESS, DECOMPRESS };
// akcje, które możliwe są do wykonania na archiwum
enum action { CREATE, ADD, EXTRACT, ERROR };

// Typ danych do reprezentacji akcji, która ma być wykonana na archiwum
typedef struct {
   int action;                  // wybrane działanie na archiwum
   int direction;               // definiuje kierunek, w którym zostanie podjęta akcja
} action_t;

// definuje typ służący do opisywania nazwy i wielkości pliku w archiwum
typedef struct {
   char Name[MAX_NAME];
   unsigned int Size;
} meta;

// typ pomocniczy do odróżniania archiwum od zwykłego pliku
typedef FILE* archive;

/*
 * Tworzy archiwum z plikami na podstawie wyspecyfikowanych szczegółów.
 *
 * Parametry:
 *    arch: archiwum, do którego zapisane mają zostać pliki
 *    params: parametry, według których zostanie utworzone archiwum, pochodzą
 *         z przetworzonych opcji, podanych przez użytkownika
 *
 * Zwraca:
 *    0 lub l.całk. dod.: wartość poprawnie zapakowanych plików
 *    -1: w przypadku błędu
 */
int create_arc_with_files(archive arch, archive_data *params);

/*
 * Dodaje do archiwum pliki na podstawie wyspecyfikowanych szczegółów.
 *
 * Parametry:
 *    arch: archiwum, do którego dodane mają zostać pliki
 *    params: parametry, według których przeprowadzona zostanie operacja,
 *            pochodzą z przetworzonych opcji, podanych przez użytkownika
 *
 * Zwraca:
 *    0 lub l.całk. dod.: wartość poprawnie dodanych do archiwum plików
 *    -1: w przypadku błędu
 */
int add_files_to_arc(archive arch, archive_data *params);

/*
 * Wypakowuje z archiwum pliki na podstawie wyspecyfikowanych szczegółów.
 *
 * Parametry:
 *    arch: archiwum, z którego mają zostać wypakowane pliki
 *    params: parametry, według których przeprowadzona zostanie operacja,
 *            pochodzą z przetworzonych opcji, podanych przez użytkownika
 *
 * Zwraca:
 *    0 lub l.całk. dod.: wartość poprawnie wypakowanych z archiwum plików
 *    -1: w przypadku błędu
 */
int extract_files_form_arc(archive arch, archive_data *params);

/*
 * Funkcja, której zadaniem jest wybór akcji, jaka zostanie wykonana na archiwum.
 *
 * Parametry:
 *    arch: archiwum
 *    act: parametr zawierający akcję oraz jej kierunek
 *    params: parametry, według których przeprowadzona zostanie operacja,
 *            pochodzą z przetworzonych opcji, podanych przez użytkownika
 *
 * Zwraca:
 *    0 : sukces
 *    -1: w przypadku błędu
 */
int make_action_on_archive(archive arch, action_t *act, archive_data *params);

#endif // ARCHIVE_H


