#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "rle.h"
#include <string.h>

enum comp {
   NONE, RLE, HUFF
};

FILE * compress_by_option(FILE *inFile, int option, int direction);

#endif // COMPRESSION_H
