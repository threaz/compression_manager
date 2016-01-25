#include <stdio.h>

#include "option.h"
#include "archive.h"

int main(int argc, char *argv[])
{
   archive_data data; // miejsce na parametry
   archive arch;      // nowe archiwum
   action_t act;      // akcja, która zostanie wykonana na archiwum

   // inicjalizuje początkowe opcje
   option_t options = init_option();

   if(argc <= 1)
   {
      fprintf(stderr, "Niepoprawny format wywolania. Uzyj opcji -h aby uzyskac pomoc.\n");
      return -1;
   }

   // "wyłuskuje" opcje z argv i zapisuje dane w options
   if(extract_options(argc, argv, &options) != 0)
      return -1;

   data = get_archive_data(options);
   act  = get_archive_action(options);
   arch = NULL;

   // wykonaj act na arch używając danych z data
   make_action_on_archive(arch, &act, &data);

   return 0;
}
