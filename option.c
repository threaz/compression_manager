#include "option.h"

option_t init_option()
{
   option_t newOpt;
   newOpt.ar_name_buf[0] = '\0';
   newOpt.buf.name_pos = 0;
   newOpt.nfiles = 0;
   newOpt.action = ERROR;
   newOpt.compression = NONE;
   newOpt.direction = NOWHERE;

   return newOpt;
}

int extract_options(int argc, char **argv, option_t *options)
{
   int tmp, get_name, get_comp, was_request;

   get_comp = 0;
   get_name = 0;
   was_request = 0;

   if(argc <= 1)
   {
      fprintf(stderr, "Niepoprawny format wywolania. Uzyj opcji -h aby uzyskac pomoc.\n");
      return -1;
   }

   for(int i = 1; i < argc; ++i)
   {
      tmp = argv[i][0];
      if(tmp == '-') // opcja
      {
         switch(tmp = argv[i][1])
         {
         case 'c':
            options->action = CREATE;
            was_request = 1;
            get_name    = 1;
            break;
         case 'a':
            options->action = ADD;
            was_request = 1;
            get_name    = 1;
            break;
         case 'e':
            options->action = EXTRACT;
            was_request = 1;
            get_name    = 1;
            break;
         case 'x':
            get_comp = 1;
            break;
         case 'h':
            yield_usage();
            return 1;
         default:
            fprintf(stderr, "Nieznana opcja.\n");
            return -1;
         }
      }
      else // nazwa lub coś niespodziewanego
      {
         if(strlen(argv[i]) + 1 > MAX_NAME)
         {
            fprintf(stderr, "Wprowadzona nazwa pliku [%s] przekracza maksymalna dozwolona dlugosc [%d]\n",
                    argv[i], MAX_NAME);
            return -1;
         }
         if(get_name) // wczytaj nazwe archiwum
         {
            strncpy(options->ar_name_buf, argv[i], MAX_NAME);
            get_name = 0;
         }
         else if(get_comp) // wczytaj typ kompresji
         {
            int tmp = atoi(argv[i]);
            if(tmp != 0 && tmp != 1 && tmp != 2 && tmp != 3 && tmp != 4)
            {
               fprintf(stderr, "Nieznany typ kompresji\n");
               return -1;
            }

            // ustaw typ kompresji
            options->compression = tmp;
            get_comp = 0;
         }
         else // gromadzi wskazniki do nazw plikow
         {
            options->buf.name_buf[options->buf.name_pos++] = argv[i];
            ++(options->nfiles);
            if(options->nfiles > MAX_FILES)
            {
               fprintf(stderr, "Przekroczono maks. dopuszczalna ilosc plikow."
                       "Pozostałe nie beda pakowane\n");
               break;
            }
         }
      }
   }

   if(! was_request || strlen(options->ar_name_buf) <= 0) // jeżeli nie wyspecyfikowano odpowiedniego trybu [c|e|a]
   {
      fprintf(stderr, "Niepoprawny format wywolania. Uzyj opcji -h aby uzyskac pomoc.\n");
      return -1;
   }
   return 0;
}

archive_data get_archive_data(option_t options)
{
   archive_data newData;

   strncpy(newData.ar_name_buf, options.ar_name_buf, MAX_NAME);
   newData.buf = options.buf;
   newData.nfiles = options.nfiles;
   newData.compression = options.compression;

   return newData;
}

action_t get_archive_action(option_t options)
{
   action_t newAct;
   newAct.action = options.action;
   newAct.direction = options.direction;

   return newAct;
}

void yield_usage()
{
   printf("Program do kompresji plikow.\n"
          "Format wywolania: kompresuj [-cae nazwa_archiwum] [nazwa_pliku ...] (-x rodzaj_kompresji)\n"
          "-c nazwa_archiwum\n"
          "\tTworzy archiwum o wskazanej nazwie.\n"
          "-e nazwa_archiwum\n"
          "\tWypakowuje plik(i) z archiwum o wskazanej nazwie.\n"
          "-a nazwa_archiwum\n"
          "\tDodaje plik(i) do archiwum o wskazanej nazwie.\n"
          "-x rodzaj_kompresji\n"
          "\tSpecyfikuje algorytm uzywany do kompresji pliku.\n"
          "\t0 - brak kompresji(domyslny)\t1 - RLE\n\t2 - Kodowanie Huffmana\t3 - LZ77/LZSS\n\t4 - LZ78/LZW\n");
}
