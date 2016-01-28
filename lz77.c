#include "lz77.h"

static unsigned char dictionary[MAX_DICT];
static unsigned char buffer[MAX_BUF];

match_data_t findPattern(unsigned int dict_pos, unsigned int buf_pos)
{
   match_data_t mData = {0, 0};
   unsigned int i, j;

   i = dict_pos; j = 0;
   while(true)
   {
      // znaleziono pierwszą literę
      if(dictionary[i] == buffer[buf_pos])
      {
         j = 1;
         // słownik i bufor są cykliczne, tak po nich przechodzimy
         while(dictionary[(i+j) % MAX_DICT] == buffer[(j+buf_pos) % MAX_BUF])
         {
            // szukaj dłuższego stringa
            // ...
            if(j >= MAX_BUF) // najdłuższy możliwy, przerywamy
               break;
            ++j;
         }

         if(mData.data_length < j)
         {
            mData.data_length = j;
            mData.data_offset = i;
         }
      }

      i = (i + 1) % MAX_DICT;

      // wróciliśmy do miejsca, gdzie zaczęliśmy
      if(i == dict_pos)
         break;
   }

   return mData;
}

void lz77_encode(FILE *inFile, FILE *outFile)
{
   unsigned int dict_pos, buf_pos;
   unsigned int i, in_buf;
   int ch;
   match_data_t mData;

   // skojarz bfp z plikiem wyjściowym
   bit_file_t *bfp = BitFile_make_new(outFile, WRITE);

   dict_pos = buf_pos = 0;

   // wypełnij bufor
   for(in_buf = 0; in_buf < MAX_BUF && (ch = fgetc(inFile)) != EOF; ++in_buf)
      buffer[in_buf] = ch;

   // w pliku nie było żadnego znaku
   if(! in_buf)
      return;

   // wypełnij cały słownik znakiem startowym
   memset(dictionary, START_SIGN, MAX_DICT * sizeof(unsigned char));

   // znajdź pasujący fragment
   mData = findPattern(dict_pos, buf_pos);

   // dopóki w buforze coś zalega
   while(in_buf > 0)
   {
      if(mData.data_length > in_buf) // żeby nie zakodować więcej niż mamy
         mData.data_length = in_buf;

      if(mData.data_length <= MAX_UNCODED) // zbyt mało żeby zakodować "wskaźnik"
      {
         BitFile_write_bit(UNCODED, bfp); // zapisz bit niezakodowanego znaku
         BitFile_write_char(buffer[buf_pos], bfp); // wpisz aktualnie pierwszy znak z bufora

         mData.data_length = 1;
      }
      else // kodujemy wskaźnik
      {
         unsigned char tmp, new_len;
         BitFile_write_bit(ENCODED, bfp);

         tmp = 0;
         tmp |= ((mData.data_offset & 4095) >> 4); // weź 8 pierwszych bitów z offsetu
         BitFile_write_char(tmp, bfp);

         tmp = 0;
         new_len = mData.data_length - (MAX_UNCODED+1);
         // pozostałe 4 z offset i 4 z length
         tmp |= (((mData.data_offset & 15) << 4) | (new_len & 15));
         BitFile_write_char(tmp, bfp);
      }

      ch = 0;
      // wpisz zakodowane znaki do slownika, a nowe do bufora
      for(i = 0; (i < mData.data_length) && (ch = fgetc(inFile)) != EOF; ++i)
      {
         dictionary[dict_pos] = buffer[buf_pos];
         buffer[buf_pos] = ch;
         dict_pos = (dict_pos+1) % MAX_DICT;
         buf_pos  = (buf_pos+1) % MAX_BUF;
      }

      // gdyby napotkany został EOF, obsłuż data_length ostatnich znaków
      if(feof(inFile))
         while(i < mData.data_length)
         {
            dictionary[dict_pos] = buffer[buf_pos];
            dict_pos = (dict_pos+1) % MAX_DICT;
            buf_pos  = (buf_pos+1) % MAX_BUF;
            --in_buf;

            if(in_buf == 0)
               break;
            ++i;
         }

      // znajdź pasujący fragment
      mData = findPattern(dict_pos, buf_pos);
   }
   // wypisz pozostałe w buforze dane
   BitFile_flush(bfp);
}

void lz77_decode(FILE *inFile, FILE *outFile)
{
   unsigned int dict_pos;
   int ch, bit;
   int offset, length;

   // skojarz bfp z plikiem wejściowym
   bit_file_t *bfp = BitFile_make_new(inFile, READ);

   // wypełnij cały słownik znakiem startowym
   memset(dictionary, START_SIGN, MAX_DICT * sizeof(unsigned char));

   dict_pos = 0;

   while((bit = BitFile_read_bit(bfp)) != EOF)
   {
      if(bit) // trafiliśmy na zakodowany znak
      {
         // offset czytamy na raty, OFFSET = 8 + 4
         offset = BitFile_read_nbits(8, bfp);
         offset <<= 4;
         offset |= BitFile_read_nbits(4, bfp);

         length = BitFile_read_nbits(LENGTH, bfp);

         if(offset == EOF || length == EOF)
            break;

         // aktualizuję długość, bo kodujemy przynajmniej 3 bajty
         length += MAX_UNCODED + 1;

         for(int i = 0; i < length; ++i)
         {
            // weź znak ze słownika
            ch = dictionary[(offset+i) % MAX_DICT];
            // wypisz go na wyjście
            fputc(ch, outFile);
            // zapisz do bufora
            buffer[i] = ch;
         }

         // przepisz z bufora do słownika
         for(int i = 0; i < length; ++i)
            dictionary[(dict_pos+i) % MAX_DICT] = buffer[i];

         // aktualizuj pozycję w słowniku
         dict_pos = (dict_pos+length) % MAX_DICT;
      }
      else // znak niezakodowany, przeczytaj go i zwroc na wyjście
      {
         ch = BitFile_read_char(bfp);
         if(ch == EOF)
            break;
         fputc(ch, outFile);
         // zapisz do słownika
         dictionary[dict_pos] = ch;
         // aktualizuj pozycję
         dict_pos = (dict_pos + 1) % MAX_DICT;
      }
   }

   return;
}

