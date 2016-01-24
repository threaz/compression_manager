#ifndef LZ77_H
#define LZ77_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bitfile.h"

#define ENCODED 1
#define UNCODED 0

#define START_SIGN ' '

#define OFFSET 12
#define LENGTH  4

#define MAX_DICT (1 << OFFSET) // 2^(12)
#define MAX_UNCODED 2
#define MAX_BUF  ((1 << LENGTH) + MAX_UNCODED)

typedef struct
{
   unsigned int data_offset;
   unsigned int data_length;
} match_data_t;

void lz77_encode(FILE *inFile, FILE *outFile);
void lz77_decode(FILE *inFile, FILE *outFile);

#endif // LZ77_H
