#include "archive.h"
#include "option.h"

int create_arc_with_files(archive arch, archive_data *params)
{
   FILE *fp;
   FILE *file_buf[MAX_FILES];   // bufor na otwarte pliki

   meta Header[params->nfiles];
   unsigned int pckd_correct = 0; // ile zapakowanych poprawnie

   for(unsigned int i = 0; i < params->nfiles; ++i)
   {
      if ((fp = fopen(params->buf.name_buf[i], "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna otworzyć jednego z plikow [%s]\n",
                 params->buf.name_buf[i]);
         continue;
      }
      else
      {
         // zapisz nazwę pliku
         strncpy(Header[pckd_correct].Name, params->buf.name_buf[i], MAX_NAME);

         // stworz plik, do którego zapiszę skompresowane dane[bądź nie ]
         FILE *tmp = tmpfile();

         // skompresuj plik ze względu na zmienną compression
         if(compress_with_params(fp, tmp, params))
         {
            fprintf(stderr, "blad przy kompresji pliku [%s]\n", params->buf.name_buf[i]);
            return -1;
         }

         // TODO: sprawdź to
         // fclose(fp);

         // znajdź rozmiar tego pliku
         fseek(tmp, 0L, SEEK_END);
         Header[pckd_correct].Size = ftell(tmp);
         fseek(tmp, 0L, SEEK_SET);

         // zapisz wskaznik pliku do tablicy
         file_buf[pckd_correct] = tmp;
         ++pckd_correct;
      }
   }

   if(! pckd_correct)
   {
      fprintf(stderr, "nie udalo sie spakowac zadnego pliku :( \n");
      return -1;
   }

   fwrite(&pckd_correct, sizeof(int), 1, arch);           // zapisz ilosc odczytanych archiwow
   fwrite(&(params->compression), sizeof(int), 1, arch);  // zapisz rodzaj kompresji

   for(unsigned int i = 0; i < pckd_correct; ++i)
   {
      fwrite(&Header[i], sizeof(meta), 1, arch);          // zapisz strukturę z metadanymi

      fp = file_buf[i];

      // przekopiuj plik do archiwum
      copy_files(fp, arch);

      fclose(fp);
   }

   return pckd_correct;
}

int add_files_to_arc(archive arch, archive_data *params)
{
   meta Header[params->nfiles];
   FILE *fp, *file_buf[params->nfiles];
   unsigned int already_packed;

   // ile jest zapakowanych
   fread(&already_packed, sizeof(int), 1, arch);
   // rodzaj kompresji
   fread(&(params->compression), sizeof(int), 1, arch);
   // ustaw wskaznik na początek pliku
   fseek(arch, 0L, SEEK_SET);
   // zaktualizuj liczbę plików
   already_packed += params->nfiles;
   // wpisz tę ilość do archiwum
   fwrite(&already_packed, sizeof(int), 1, arch);
   // ustaw wskaznik na koniec pliku
   fseek(arch, 0L, SEEK_END);

   // otworz potrzebne do dodania pliki
   unsigned int succ_opened = 0;
   for(unsigned int i = 0; i < params->nfiles; ++i)
   {
      if ((fp = fopen(params->buf.name_buf[i], "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna otworzyć jednego z plikow [%s]\n",
                 params->buf.name_buf[i]);
         continue;
      }
      else
      {
         // dodaj nazwe do naglowka
         strncpy(Header[succ_opened].Name, params->buf.name_buf[i], MAX_NAME);

         // stworz plik, do którego zapiszę skompresowane dane[bądź nie ]
         FILE *tmp = tmpfile();

         // skompresuj plik ze względu na zmienną compression
         if(compress_with_params(fp, tmp, params))
         {
            fprintf(stderr, "blad przy kompresji pliku [%s]\n", params->buf.name_buf[i]);
            return -1;
         }

         // TODO: czy mogę zamknąć
         // fclose(fp);

         // przewiń na początek
         fseek(tmp, 0L, SEEK_SET);

         // znajdź rozmiar tego pliku i dodaj do nagłówka
         fseek(tmp, 0L, SEEK_END);
         Header[succ_opened].Size = ftell(tmp);
         fseek(tmp, 0L, SEEK_SET);

         // zapisz wskaznik pliku do tablicy
         file_buf[succ_opened] = tmp;
         ++succ_opened;
      }
   }

   // dodaj je do archiwum
   for(unsigned int i = 0; i < succ_opened; ++i)
   {
      fwrite(&Header[i], sizeof(meta), 1, arch); // zapisz strukturę z metadanymi

      fp = file_buf[i];

      // przekopiuj plik do archiwum
      copy_files(fp, arch);
      fclose(fp);
   }
   return succ_opened;
}

int extract_files_form_arc(archive arch, archive_data *params)
{
   FILE *fp;
   unsigned int already_packed = 0;
   unsigned int succ_extracted = 0;

   // ile jest zapakowanych
   fread(&already_packed, sizeof(int), 1, arch);
   // rodzaj kompresji
   fread(&params->compression, sizeof(int), 1, arch);

   meta Header[already_packed];
   size_t pos_in_file[already_packed];

   for(unsigned int i = 0; i < already_packed; ++i)
   {
      // importuj metadane
      fread(&Header[i], sizeof(meta), 1, arch);
      // zapisz poczatek pliku
      pos_in_file[i] = ftell(arch);
      // skocz do metadanych kolejnego pliku
      fseek(arch, (long)Header[i].Size, SEEK_CUR);
   }
   // wyszukaj plikow, ktore maja byc wypakowane
   // jezeli znaleziony - wypakuj
   // jezeli nie - wypisz blad na stderr
   for(unsigned int i = 0; i < params->nfiles; ++i)
   {
      for(unsigned int j = 0; j < already_packed; ++j)
         // znaleziono plik o takiej nazwie w archiwum
         if(strcmp(params->buf.name_buf[i], Header[j].Name) == 0)
         {
            // przejdz do poczatku pliku w archiwum
            fseek(arch, pos_in_file[j], SEEK_SET);

            FILE *tmp = tmpfile();

            // skopiuj plik
            char buffer[2048];
            size_t bytes = 0, total = 0;

            // jeśli plik jest mniejszy/równy buforowi
            if(Header[j].Size <= 2048) {
               fread(buffer, 1, (size_t) Header[j].Size, arch);
               fwrite(buffer, 1, (size_t) Header[j].Size, tmp);
            }
            else
            {
               while(total < Header[j].Size)
               {
                  if(Header[j].Size - total > 2048) {
                     fread(buffer, 1, 2048, arch);
                     fwrite(buffer, 1, 2048, tmp);
                     total += 2048;
                  }
                  else {
                     bytes = fread(buffer, 1, (size_t) Header[j].Size - total, arch);
                     fwrite(buffer, 1, bytes, tmp);
                     total += bytes;
                     break;
                  }
               }
            }

            // ustaw wskaznik na poczatku pliku
            fseek(tmp, 0L, SEEK_SET);

            FILE *tmp1 = tmpfile();

            // dekompresuj plik

            // skompresuj plik ze względu na zmienną compression
            if(decompress_with_params(tmp, tmp1, params))
            {
               fprintf(stderr, "blad przy dekompresji pliku [%s]\n", params->buf.name_buf[i]);
               return -1;
            }

            fclose(tmp);

            // utwórz plik
            if((fp = fopen(Header[j].Name, "wb")) == NULL)
            {
               fprintf(stderr, "blad, przy probie utworzenia pliku [%s]\n",
                       Header[i].Name);
               continue;
            }

            fseek(tmp1, 0L, SEEK_SET);

            // kopiuj zdekompresowany plik do właściwego miejsca
            copy_files(tmp1, fp);

            fclose(tmp1);
            ++succ_extracted;
         }
   }
   return succ_extracted;
}

int make_action_on_archive(archive arch, action_t *act, archive_data *params)
{
   if(act->action == CREATE)
   {
      // otworz archiwum
      if((arch = fopen(params->ar_name_buf, "wb")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", params->ar_name_buf);
         return -1;
      }

      create_arc_with_files(arch, params);
   }
   else if(act->action == ADD)
   {
      // otworz archiwum
      if((arch = fopen(params->ar_name_buf, "rb+")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", params->ar_name_buf);
         return -1;
      }

      add_files_to_arc(arch, params);
   }
   else if(act->action == EXTRACT)
   {
      // otworz archiwum
      if((arch = fopen(params->ar_name_buf, "rb")) == NULL)
      {
         fprintf(stderr, "nie mozna utworzyć archiwum %s\n", params->ar_name_buf);
         return -1;
      }

      extract_files_form_arc(arch, params);
   }
   return 0;
}

