#ifndef BITFILE_H
#define BITFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define UCHAR_BITS (sizeof(unsigned char)*8)
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

typedef enum {
   READ,
   WRITE
} BF_MODE;

typedef struct
{
    FILE *fp;
    unsigned char bitBuffer;
    unsigned char bitLeft;
    BF_MODE mode;
} bit_file_t;

bit_file_t *BitFile_make_new(FILE *stream, BF_MODE mode);

int BitFile_write_bit(int bit, bit_file_t *bfp);
int BitFile_read_bit(bit_file_t *bfp);

int BitFile_write_char(unsigned char ch, bit_file_t *bfp);
int BitFile_read_char(bit_file_t *bfp);

int BitFile_flush(bit_file_t *bfp);

#endif // BITFILE_H
