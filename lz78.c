#include "lz78.h"

dict_t *init_dictionary()
{
   dict_t *dp = (dict_t*)malloc(sizeof(dict_t));

   dp->pos = EOF_CODE+1;
   for(int i = 0; i < EOF_CODE+1; ++i) {
      dp->entry[i].codeWord   = i;
      dp->entry[i].prefixWord = CHAR_ENTRY;
   }

   return dp;
}

// potrzeba zewnetrzenego spawdzenia, czy nie jest pelny
void add_entry_to_dictionary(entry_t ent, dict_t *dict)
{
   dict->entry[dict->pos++] = ent;
}

void clear_dictionary(dict_t *dict)
{
   dict->pos = EOF_CODE+1;
}

bool is_full(dict_t *dict)
{
   return dict->pos == MAX_DICT_S;
}

bool is_entry(entry_t ent, dict_t *dict)
{
   for(unsigned int i = 0; i < dict->pos; ++i)
      if(ent.codeWord == dict->entry[i].codeWord &&
            ent.prefixWord == dict->entry[i].prefixWord)
         return true;
   return false;
}

bool is_entry_index(int index, dict_t *dict)
{
   return (index < dict->pos);
}

int get_entry_index(entry_t ent, dict_t *dict)
{
   for(unsigned int i = 0; i < dict->pos; ++i)
      if(ent.codeWord == dict->entry[i].codeWord &&
            ent.prefixWord == dict->entry[i].prefixWord)
         return i;
   return -1;
}

entry_t get_entry(int index, dict_t *dict)
{
   return dict->entry[index];
}

entry_t make_entry(unsigned pref, unsigned word)
{
   entry_t ent;
   ent.codeWord   = word;
   ent.prefixWord = pref;

   return ent;
}

void encode_entry(entry_t ent, dict_t *dict, bit_file_t *bfp)
{
   int index = get_entry_index(ent, dict);
   // zapisz pierwsze 4 bity z 12
   for(int i = 11; i >= 8; --i)
      BitFile_write_bit(CHECK_BIT(index, i), bfp);

   // zapisz pozostałe 8 bitów
   BitFile_write_char(index, bfp);
}

int get_first_char(entry_t ent, dict_t *dict)
{
   if(ent.prefixWord == CHAR_ENTRY)
      return ent.codeWord;
   return get_first_char(dict->entry[ent.prefixWord], dict);
}

int get_codeword(bit_file_t *bfp)
{
   int code_word = BitFile_read_nbits(4, bfp);
   code_word <<= UCHAR_BITS;
   code_word |= BitFile_read_char(bfp);

   return code_word;
}

void decode_entry(entry_t ent, dict_t *dict, FILE *outFile)
{
   if(ent.prefixWord == CHAR_ENTRY)
   {
      // doszliśmy do pierwszej litery
      fputc(ent.codeWord, outFile);
      return;
   }
   decode_entry(dict->entry[ent.prefixWord], dict, outFile);
   fputc(ent.codeWord, outFile);
}

void lz78_encode(FILE *inFile, FILE *outFile)
{
   dict_t *dict = init_dictionary();
   entry_t t_ent, p_ent;
   int ch;

   // skojarz bfp z plikiem wyjsciowym
   bit_file_t *bfp = BitFile_make_new(outFile, WRITE);

   if((ch = fgetc(inFile)) == EOF)
      return;

   // tworzy poczatkowy wpis
   t_ent = make_entry(CHAR_ENTRY, ch);
   while((ch = fgetc(inFile)) != EOF)
   {
      if(is_full(dict))
         clear_dictionary(dict);
      // znajdz indeks poprzedniego wpisu
      int index = get_entry_index(t_ent, dict);
      // utworz nowy
      p_ent     = make_entry(index, ch);
      // poszukaj nowoutworzonego symbolu w slowniku
      index = get_entry_index(p_ent, dict);
      if(index < 0) // brak wpisu w slowniku
      {
         // zakoduj poprzedni wpis
         encode_entry(t_ent, dict, bfp);
         // dodaj nowy do słownika
         add_entry_to_dictionary(p_ent, dict);
         // ostatnia litera rozpoczyna nowy wpis
         t_ent = make_entry(CHAR_ENTRY, ch);
      }
      else // znaleziono w slowniku
         t_ent = p_ent;
   }
   // zakoduj jeszcze ostatni wpis
   encode_entry(t_ent, dict, bfp);
   // kodujemy też EOF
   if(ch == EOF)
   {
      entry_t eof = dict->entry[EOF_CODE];
      encode_entry(eof, dict, bfp);
   }
   // wyczyść bufor
   BitFile_flush(bfp);
}

void lz78_decode(FILE *inFile, FILE *outFile)
{
   dict_t *dict = init_dictionary();
   entry_t string, entry, p_ent;
   int code;

   // skojarz bfp z plikiem wyjsciowym
   bit_file_t *bfp = BitFile_make_new(inFile, READ);

   // weź pierwszy kod z wejścia
   code = get_codeword(bfp);
   if(code == EOF_CODE)
      return;

   // stwórz wpis
   string = make_entry(CHAR_ENTRY, code);
   // wypisz go
   decode_entry(string, dict, outFile);

   while((code = get_codeword(bfp)) != EOF_CODE)
   {
      if(is_full(dict))
         clear_dictionary(dict);
      if(! is_entry_index(code, dict)) // specjalny przypadek
                                       // brak wpisu w słowniku
      {
         int index = get_entry_index(string, dict);
         entry = make_entry(index, get_first_char(string, dict));
      }
      else // w słowniku jest taki wpis
         entry = get_entry(code, dict);
      // wypisujemy wpis do pliku
      decode_entry(entry, dict, outFile);
      // dodaj nowy wpis do slownika
      int index = get_entry_index(string, dict);
      // tworzymy go jako string + pierwsza litera entry
      p_ent = make_entry(index, get_first_char(entry, dict));
      add_entry_to_dictionary(p_ent, dict);
      string = entry;
   }

}
