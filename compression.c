#include "compression.h"

int compress_by_option(FILE *inFile, FILE **outFile, int option)
{
   FILE *fp;

   if(option == RLE)
   {
      // stworz plik tymczasowy
      if((fp = tmpfile()) == NULL)
      {
         fprintf(stderr, "nie mozna stworzyc pliku tymczasowego\n");
         return -1;
      }
      // do fp zapisz skompresowany plik
      rle_encode(inFile, fp);
      *outFile = fp;
   }
   else if(option == NONE)
      *outFile = inFile;

   return 0;
}
