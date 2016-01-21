#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdlib.h>

#include "archive.h"
#include "rle.h"
#include "huffman.h"
#include "lz77.h"

// definiuje używane typy kompresji

enum comp {
   NONE, RLE, HUFF, LZ77, LZ78
};

int copy_files(FILE *inFile, FILE *outFile);
int compress_with_params(FILE *inFile, FILE *outFile, archive_data *params);
int decompress_with_params(FILE *inFile, FILE *outFile, archive_data *params);

#endif // COMPRESSION_H
