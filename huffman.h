#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define HUFF_EOF UCHAR_MAX+1

typedef struct bit_buffer
{
   unsigned char buffer;
   int pos;
} bit_buffer;

typedef struct NODE *pNODE;
typedef struct NODE
{
   int  val;
   unsigned long long nelem;
   unsigned char label;
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

int get_value(qpNODE node);
int add_elem(QUEUE *que, pNODE elem);
int add_elem_with_sort(QUEUE *que, pNODE elem);
void print_queue(QUEUE *que);
pNODE remove_elem(QUEUE *que);

pNODE create_huffman_tree(unsigned long long *letter_cnt);
void huffman_encode(FILE *inFile, FILE *outFile);

#endif // HUFFMAN_H
