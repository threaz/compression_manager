#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bitfile.h"

#define HUFF_EOF UCHAR_MAX+1
#define INT_BITS (sizeof(int)*8)

typedef struct NODE *pNODE;
typedef struct NODE
{
   unsigned char  val;
   unsigned long long nelem;
   int label;
   int label_len;
   pNODE left;
   pNODE right;
} NODE;

typedef struct qNODE *qpNODE;
typedef struct qNODE
{
   pNODE elem;
   qpNODE next;
} qNODE;

typedef struct QUEUE
{
   qpNODE first;
   qpNODE last;
   int nelem;
} QUEUE;

pNODE make_node(char val);
QUEUE new_queue();

int get_nelem(qpNODE node);
int add_elem(QUEUE *que, pNODE elem);
int add_elem_with_sort(QUEUE *que, pNODE elem);
void print_queue(QUEUE *que);
pNODE remove_elem(QUEUE *que);

pNODE find_char_node(int ch, pNODE *map);
pNODE create_huffman_tree(unsigned long long *letter_cnt);

void huffman_encode(FILE *inFile, FILE *outFile);
void huffman_decode(FILE *inFile, FILE *outFile);

#endif // HUFFMAN_H
