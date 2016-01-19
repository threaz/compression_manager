#include "huffman.h"
#include "lz77.h"
#include "bitfile.h"

//int main()
//{
//   QUEUE Que = new_queue();

//   char tab[] = "damian";
//   pNODE tab_node[10];
//   for(int i = 0; i < 6; ++i) {
//      tab_node[i] = make_node(tab[i]);
//      add_elem(&Que, tab_node[i]);
//      tab_node[i]->nelem = i;
//   }

//   print_queue(&Que);
//   tab_node[6] = make_node('x');
//   tab_node[6]->nelem = 6;
//   add_elem_with_sort(&Que, tab_node[6]);
//   print_queue(&Que);

//   FILE *fp = fopen("Makefile", "rb");
//   FILE *fp2 = fopen("karola", "wb");
//   lz77_encode(fp, fp2);

//   FILE *test = fopen("test", "rb");
//   bit_file_t *bfp;

//   bfp = BitFile_make_new(test, READ);

//   unsigned char ch;
//   for(int i = 0; i < 3; ++i)
//   {
//      BitFile_read_bit(bfp);
//      ch = BitFile_read_char(bfp);
//      putchar(ch);
//   }

//   putchar('\n');

//   BitFile_write_bit(1, bfp);
//   BitFile_write_char('A', bfp);
//   BitFile_write_bit(1, bfp);
//   BitFile_write_char('B', bfp);
//   BitFile_write_bit(1, bfp);
//   BitFile_write_char('C', bfp);

//   BitFile_flush(bfp);
//   //0110 0010
//   return 0;
//}
