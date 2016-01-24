#ifndef LZ78_H
#define LZ78_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "bitfile.h"

#define MAX_DICT_S 4095
#define CODE_WORD_S 12
#define CHAR_ENTRY UINT_MAX
#define EOF_CODE 256

typedef struct
{
   unsigned int codeWord;
   unsigned int prefixWord;
} entry_t;

typedef struct
{
   entry_t entry[MAX_DICT_S];
   unsigned int pos;
} dict_t;

dict_t *init_dictionary();
void add_entry_to_dictionary(entry_t ent, dict_t *dict);
int get_entry_index(entry_t ent, dict_t *dict);
entry_t get_entry(int index, dict_t *dict);
void clear_dictionary(dict_t *dict);
bool is_full(dict_t *dict);
bool is_entry(entry_t ent, dict_t *dict);
bool is_entry_index(int index, dict_t *dict);

int get_first_char(entry_t ent, dict_t *dict);
void encode_entry(entry_t ent, dict_t *dict, bit_file_t *bfp);
void decode_entry(entry_t ent, dict_t *dict, FILE *outFile);

entry_t make_entry(unsigned pref, unsigned word);
int get_codeword(bit_file_t *bfp);

void lz78_encode(FILE *inFile, FILE *outFile);
void lz78_decode(FILE *inFile, FILE *outFile);

#endif // LZ78_H
