#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "rle.h"
#include <string.h>

#define MAX_NAME 1000
#define MAX_FILES 100

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

enum direction { NOWHERE, COMPRESS, DECOMPRESS };
enum action { CREATE, ADD, EXTRACT, ERROR };

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
typedef FILE * archive;

int create_arc_with_files(archive arch, archive_data *params);
int add_files_to_arc(archive arch, archive_data *params);
int extract_files_form_arc(archive arch, archive_data *params);

int make_action_on_archive(FILE *archive, action_t *act, archive_data *params);

#endif // ARCHIVE_H


