#include "huffman.h"

pNODE make_node(char val)
{
   pNODE pn;
   if((pn = (pNODE)malloc(sizeof(NODE))) == NULL)
      return NULL;

   pn->left = pn->right = NULL;
   pn->val = val;
   return pn;
}

QUEUE new_queue()
{
   QUEUE que;
   que.first = que.last = NULL;
   que.nelem = 0;
   return que;
}

int get_value(qpNODE node)
{
   return node->elem->nelem   ;
}

int add_elem(QUEUE *que, pNODE elem)
{
   if(que == NULL || elem == NULL)
      return -1;

   qpNODE qpn;
   if((qpn = (qpNODE)malloc(sizeof(qNODE))) == NULL)
      return 1;

   qpn->elem = elem;
   qpn->next = NULL;

   if(que->first == NULL)
      que->first = que->last = qpn;
   else
      que->last = que->last->next = qpn;

   que->nelem++;
   return 0;
}

int add_elem_with_sort(QUEUE *que, pNODE elem)
{
   if(que == NULL || elem == NULL)
      return -1;

   qpNODE qpn;
   if((qpn = (qpNODE)malloc(sizeof(qNODE))) == NULL)
      return 1;

   qpn->elem = elem;
   qpn->next = NULL;

   int cmp_val = elem->nelem;

   if(que->first == NULL) // brak elementów
      que->first = que->last = qpn;
   else if(que->first == que->last) // jeden element
   {
      if(cmp_val <= get_value(que->first))
      {
         qpn->next = que->last;
         que->first = qpn;
      }
      else
      {
         que->first->next = qpn;
         que->last = qpn;
      }
   }
   else // więcej niż jeden element
   {
      if(cmp_val <= get_value(que->first)) // wstaw na początek
      {
         qpn->next = que->first;
         que->first = qpn;
      }
      else
      {
         qpNODE act = que->first;
         while(act != que->last && cmp_val > get_value(act->next))
            act = act->next;

         if(act == que->last) // dotarliśmy na koniec, dostaw el. na koniec
         {
            que->last->next = qpn;
            que->last = qpn;
         }
         else // wstaw element w środek
         {
            qpNODE prev, next;

            prev = act; next = act->next;
            qpn->next = act->next;
            act->next = qpn;
         }
      }
   }
   que->nelem++;
   return 0;
}

pNODE remove_elem(QUEUE *que)
{
   if(que == NULL)
      return NULL;

   qpNODE qpn;
   if(que->first == NULL)
      return NULL;
   if(que->first == que->last)
   {
      qpn = que->first;
      que->nelem--;
      que->first = que->last = NULL;
   }
   else
   {
      qpn = que->first;
      que->first = que->first->next;
      que->nelem--;
   }

   return qpn->elem;
}

void print_queue(QUEUE *que)
{
   if(que == NULL)
      return;

   qpNODE qpn = que->first;
   while(qpn)
   {
      printf((qpn->next) ? "%c -> " : "%c", qpn->elem->val);
      qpn = qpn->next;
   }
   putchar('\n');
}

int compare(const void *p1, const void *p2)
{
   return *(unsigned long long*)p1 - *(unsigned long long*)p2;
}

void label_huffman_tree_rec(pNODE root, unsigned char path, int npos, int set)
{
   if(set)
      path = path | (1 << npos); // wstaw npos-ty bit

   if(root->left == NULL && root->right == NULL)
      root->label = path;
   else
   {
      if(root->left != NULL)
         label_huffman_tree_rec(root->left, path, npos+1, 0);
      if(root->right != NULL)
         label_huffman_tree_rec(root->right, path, npos+1, 1);
   }
}

void label_huffman_tree(pNODE root)
{
   if(! root)
      return;

   if(root->left)
      label_huffman_tree_rec(root->left, 0, 1, 0);
   if(root->right)
      label_huffman_tree_rec(root->right, 0, 1, 1);
}

pNODE create_huffman_tree(unsigned long long *letter_cnt)
{
   int j;
   pNODE node_tab[UCHAR_MAX+2];
   QUEUE que = new_queue();

   for(int i = 0; i < UCHAR_MAX+2; ++i)
      node_tab[i] = NULL;

   for(j = 0; j < UCHAR_MAX+2; ++j) {
//      printf("%llu %d\n", letter_cnt[j], j);
      if(letter_cnt[j]) // jeśli jakiś znak wystąpił przynajmniej raz
      {
         node_tab[j] = make_node((char)j);
         node_tab[j]->val   = (char)j;
         node_tab[j]->nelem = letter_cnt[j];
      }
   }
   node_tab[UCHAR_MAX+1]->val = HUFF_EOF;

   for(int i = 0; i < j; ++i)
      add_elem_with_sort(&que, node_tab[i]);

   int label_len = ceil(log2(que.nelem)); // wyznacz max liczbę bitów potrzebną do kodow.

   while(que.nelem > 1) // aż w kolejce pozostanie całe drzewo huffmana
   {
      // zdejmij dwa elementy o najmniejszej liczbie wystąpień
      pNODE p1 = remove_elem(&que);
      pNODE p2 = remove_elem(&que);

      // utwórz nowy węzeł
      pNODE pn = make_node(0);
      pn->left  = p1;
      pn->right = p2;
      pn->nelem = p1->nelem + p2->nelem;

      // dodaj do kolejki, zachowując kolejność
      add_elem_with_sort(&que, pn);
   }

   // zdejmij z listy całe drzewo
   pNODE root = remove_elem(&que);
   root->label = label_len; // zapisz iloma bitami się posługujemy

   label_huffman_tree(root); // etykietuj drzewo



//   print_queue(&que);
   return root;
}

void huffman_encode(FILE *inFile, FILE *outFile)
{
   unsigned long long letter_cnt[UCHAR_MAX+2]; // ostatnim znakiem będzie EOF
   pNODE hTree;
   int ch;

   for(int i = 0; i < UCHAR_MAX+2; ++i) // zeruj liczniki
      letter_cnt[i] = 0;

   while((ch = fgetc(inFile)) != EOF) // zlicz znaki
      ++letter_cnt[ch];
   letter_cnt[UCHAR_MAX+1] = 1; // EOF

   fseek(inFile, 0L, SEEK_SET); // przewiń na początek pliku

   // tworzy drzewo Huffmana
   if((hTree = create_huffman_tree(letter_cnt)) == NULL)
   {
      fprintf(stderr, "nie udało się stworzyć drzewa huffmana\n");
      return;
   }

}

