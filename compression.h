#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "rle.h"
#include <string.h>

enum comp {
   NONE, RLE, HUFF
};

int compress_by_option(FILE *inFile, FILE **outFile, int option);

#endif // COMPRESSION_H
