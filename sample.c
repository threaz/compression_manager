#include "huffman.h"

int main()
{
   QUEUE Que = new_queue();

   char tab[] = "damian";
   pNODE tab_node[10];
   for(int i = 0; i < 6; ++i) {
      tab_node[i] = make_node(tab[i]);
      add_elem(&Que, tab_node[i]);
      tab_node[i]->nelem = i;
   }

   print_queue(&Que);
   tab_node[6] = make_node('x');
   tab_node[6]->nelem = 6;
   add_elem_with_sort(&Que, tab_node[6]);
   print_queue(&Que);

   FILE *fp = fopen("Makefile", "r");
   huffman_encode(fp, NULL);
   return 0;
}
