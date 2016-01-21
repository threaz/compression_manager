#include "option.h"

option_t init_option()
{
   option_t newOpt;
   newOpt.buf.name_pos = 0;
   newOpt.nfiles = 0;
   newOpt.action = ERROR;
   newOpt.compression = NONE;
   newOpt.direction = NOWHERE;

   return newOpt;
}

int extract_options(int argc, char **argv, option_t *options)
{
   int tmp, get_name, get_comp;

   get_comp = 0;
   get_name = 0;

   for(int i = 1; i < argc; ++i)
   {
      tmp = argv[i][0];
      if(tmp == '-') // opcja
      {
         switch(tmp = argv[i][1])
         {
         case 'c':
            options->action = CREATE;
            get_name = 1;
            break;
         case 'a':
            options->action = ADD;
            get_name = 1;
            break;
         case 'e':
            options->action = EXTRACT;
            get_name = 1;
            break;
         case 'x':
            get_comp = 1;
            break;
         default:
            fprintf(stderr, "nieznana opcja\n");
            return -1;
         }
      }
      else // nazwa
      {
         if(strlen(argv[i]) + 1 > MAX_NAME)
         {
            fprintf(stderr, "wprowadzona nazwa pliku [%s] przekracza maksymalna dozwolona dlugosc [%d]\n",
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
            if(tmp != 0 && tmp != 1 && tmp != 2 && tmp != 3)
            {
               fprintf(stderr, "nieznany typ kompresji\n");
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
               fprintf(stderr, "przekroczono maks. dopuszczalna ilosc plikow"
                       "\npozostałe nie beda pakowane\n");
               break;
            }
         }
      }
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

