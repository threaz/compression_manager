#include "option.h"
#include "metadata.h"
#include "rle.h"

#include <string.h>

static char ar_name_buf[MAX_NAME]; // przechowuje nazwę archiwum
static char *name_buf[MAX_NAME]; // bufor na nazwy plikow
static FILE *file_buf[MAX_FILES]; // bufor na otwarte pliki
static int name_pos; // aktualna pozycja w tym buforze
static int nfiles; // licznik plików
static int opt;    // wybrana opcja
static int compression; // rodzaj kompresji, domyślnie brak = 0

void analyze_options(int argc, char **argv)
{
   int tmp, get_name;
   get_name = 0;
   name_pos = 0;
   nfiles = 0;

   for(int i = 1; i < argc; ++i)
   {
      tmp = argv[i][0];
      if(tmp == '-') // opcja
      {
         switch(tmp = argv[i][1])
         {
         case 'c':
            opt = CREATE;
            get_name = 1;
            break;
         case 'a':
            opt = ADD;
            get_name = 1;
            break;
         case 'e':
            opt = EXTRACT;
            get_name = 1;
            break;
         default:
            fprintf(stderr, "nieznana opcja\n");
            exit(3);
         }
      }
      else  // nazwa
      {
         if(strlen(argv[i]) + 1 > MAX_NAME)
         {
            fprintf(stderr, "wprowadzona nazwa pliku [%s] przekracza maksymalna dozwolona dlugosc [%d]\n",
                    argv[i], MAX_NAME);
            exit(2);
         }
         if(get_name)
         {
            strncpy(ar_name_buf, argv[i], MAX_NAME);
            get_name = 0;
         }
         else // gromadzi wskazniki do nazw plikow
         {
            name_buf[name_pos++] = argv[i];
            ++nfiles;
            if(nfiles > MAX_FILES)
               break;
         }
      }
   }
}

int create_arc_with_files(FILE *archive)
{
   FILE *fp;
   Meta Header[nfiles];

   unsigned int pckd_correct = 0; // ile zapakowanych poprawnie

   for(int i = 0; i < nfiles; ++i)
   {
      if ((fp = fopen(name_buf[i], "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna otworzyć jednego z plikow [%s]\n",
                 name_buf[i]);
         continue;
      }
      else
      {
         strncpy(Header[pckd_correct].Name, name_buf[i], MAX_NAME);

         // znajdź rozmiar tego pliku
         fseek(fp, 0L, SEEK_END);
         Header[pckd_correct].Size = ftell(fp);
         fseek(fp, 0L, SEEK_SET);

         // zapisz wskaznik pliku do tablicy
         file_buf[pckd_correct] = fp;
         ++pckd_correct;
      }
   }

   if(pckd_correct == 0)
   {
      fprintf(stderr, "nie udalo sie spakowac zadnego pliku :( \n");
      exit(-2);
   }

   fwrite(&pckd_correct, sizeof(int), 1, archive); // zapisz ilosc odczytanych archiwow
   fwrite(&compression, sizeof(int), 1, archive);  // zapisz rodzaj kompresji

   for(unsigned int i = 0; i < pckd_correct; ++i)
   {
      fwrite(&Header[i], sizeof(Meta), 1, archive); // zapisz strukturę z metadanymi

      fp = file_buf[i];
      // kopiuj plik (BEZ KOMPRESJI)
      char buffer[2048];
      size_t bytes;
      while((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
         fwrite(buffer, 1, bytes, archive);
      fclose(fp);
   }

   return pckd_correct;
}

int add_files_to_arc(FILE *archive)
{
   Meta Header[nfiles];
   FILE *fp;

   unsigned int already_packed, comp;
   // ile jest zapakowanych
   fread(&already_packed, sizeof(int), 1, archive);
   // rodzaj kompresji
   fread(&comp, sizeof(int), 1, archive);
   // ustaw wskaznik na początek pliku
   fseek(archive, 0L, SEEK_SET);
   // zaktualizuj liczbę plików
   already_packed += nfiles;
   // wpisz tę ilość do archiwum
   fwrite(&already_packed, sizeof(int), 1, archive);
   // ustaw wskaznik na koniec pliku
   fseek(archive, 0L, SEEK_END);

   // otworz potrzebne do dodania pliki
   unsigned int succ_opened = 0;
   for(int i = 0; i < nfiles; ++i)
   {
      if ((fp = fopen(name_buf[i], "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna otworzyć jednego z plikow [%s]\n",
                 name_buf[i]);
         continue;
      }
      else
      {
         // dodaj nazwe do naglowka
         strncpy(Header[succ_opened].Name, name_buf[i], MAX_NAME);

         // znajdź rozmiar tego pliku i dodaj do nagłówka
         fseek(fp, 0L, SEEK_END);
         Header[succ_opened].Size = ftell(fp);
         fseek(fp, 0L, SEEK_SET);

         // zapisz wskaznik pliku do tablicy
         file_buf[succ_opened] = fp;
         ++succ_opened;
      }
   }
   // dodaj je do archiwum
   for(unsigned int i = 0; i < succ_opened; ++i)
   {
      fwrite(&Header[i], sizeof(Meta), 1, archive); // zapisz strukturę z metadanymi

      fp = file_buf[i];
      // kopiuj plik (BEZ KOMPRESJI)
      char buffer[2048];
      size_t bytes;
      while((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
         fwrite(buffer, 1, bytes, archive);
      fclose(fp);
   }
   return succ_opened;
}

int extract_files_form_arc(FILE *archive)
{
   FILE *fp;

   unsigned int already_packed, comp;
   unsigned int succ_extracted = 0;

   // ile jest zapakowanych
   fread(&already_packed, sizeof(int), 1, archive);
   // rodzaj kompresji
   fread(&comp, sizeof(int), 1, archive);

   Meta Header[already_packed];
   size_t pos_in_file[already_packed];
   for(unsigned int i = 0; i < already_packed; ++i)
   {
      // importuj metadane
      fread(&Header[i], sizeof(Meta), 1, archive);
      // zapisz poczatek pliku
      pos_in_file[i] = ftell(archive);
      // skocz do metadanych kolejnego pliku
      fseek(archive, (long)Header[i].Size, SEEK_CUR);
   }
   // wyszukaj plikow, ktore maja byc wypakowane
   // jezeli znaleziony - wypakuj
   // jezeli nie - wypisz blad na stderr
   for(int i = 0; i < nfiles; ++i)
   {
      for(unsigned int j = 0; j < already_packed; ++j)
         // znaleziono plik o takiej nazwie w archiwum
         if(strcmp(name_buf[i], Header[j].Name) == 0)
         {
            // przejdz do poczatku pliku w archiwum
            fseek(archive, pos_in_file[j], SEEK_SET);
            // utworz plik
            if((fp = fopen(Header[j].Name, "wb")) == NULL)
            {
               fprintf(stderr, "blad, przy probie utworzenia pliku [%s]\n",
                       Header[i].Name);
               exit(3);
            }

            // skopiuj plik
            char buffer[2048];
            size_t bytes = 0, total = 0;

            // jeśli plik jest mniejszy/równy buforowi
            if(Header[j].Size <= 2048) {
               fread(buffer, 1, (size_t) Header[j].Size, archive);
               fwrite(buffer, 1, (size_t) Header[j].Size, fp);
            }
            else
            {
               while(total < Header[j].Size)
               {
                  if(Header[j].Size - total > 2048) {
                     fread(buffer, 1, 2048, archive);
                     fwrite(buffer, 1, 2048, fp);
                     total += 2048;
                  }
                  else {
                     bytes = fread(buffer, 1, (size_t) Header[j].Size - total, archive);
                     fwrite(buffer, 1, bytes, fp);
                     total += bytes;
                     break;
                  }
               }
            }

            fclose(fp);
            succ_extracted++;
         }
   }
   return succ_extracted;
}

int make_action()
{
   FILE *archive;

   if(opt == CREATE)
   {
      // otworz archiwum
      if((archive = fopen(ar_name_buf, "wb")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", ar_name_buf);
         exit(-1);
      }

      create_arc_with_files(archive);
   }
   else if(opt == ADD)
   {
      // otworz archiwum
      if((archive = fopen(ar_name_buf, "rb+")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", ar_name_buf);
         exit(-1);
      }

      add_files_to_arc(archive);
   }
   else if(opt == EXTRACT)
   {
      // otworz archiwum
      if((archive = fopen(ar_name_buf, "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", ar_name_buf);
         exit(-1);
      }

      extract_files_form_arc(archive);
   }
   return 0;
}
