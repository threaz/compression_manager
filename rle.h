#ifndef RLE_H
#define RLE_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

void rle_encode(FILE *inFile, FILE *outFile);
void rle_decode(FILE *inFile, FILE *outFile);

#endif // RLE_H
