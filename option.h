#ifndef OPTION_H
#define OPTION_H

#include "archive.h"
#include "compression.h"
#include <string.h>

typedef struct
{
   char ar_name_buf[MAX_NAME];  // przechowuje nazwę archiwum
   name_buffer  buf;            // bufor na nazwy plikow
   unsigned int nfiles;         // licznik plików
   unsigned int compression;    // rodzaj kompresji, domyślnie powinien być NONE

   int action;                  // wybrane działanie na archiwum
   int direction;               // definiuje kierunek, w którym zostanie podjęta akcja
} option_t;

option_t init_option();
int extract_options(int argc, char **argv, option_t *options);

archive_data get_archive_data(option_t options);
action_t     get_archive_action(option_t options);

#endif // OPTION_H
