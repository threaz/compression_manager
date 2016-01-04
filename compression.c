#include "compression.h"

FILE * compress_by_option(FILE *inFile, int option, int direction)
{
   FILE *fp;

   if(option == RLE)
   {
      // stworz plik tymczasowy
      if((fp = tmpfile()) == NULL)
      {
         fprintf(stderr, "nie mozna stworzyc pliku tymczasowego\n");
         return NULL;
      }

      if(direction == 1) // kompresja
      // do fp zapisz skompresowany plik
         rle_encode(inFile, fp);
      else if(direction == -1) // dekompresja
      {
         puts("mamam");
         rle_decode(inFile, fp);
      }
   }
   else if(option == NONE)
      return inFile;

   return fp;
}
