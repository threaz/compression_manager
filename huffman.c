#include "huffman.h"

pNODE make_node(char val)
{
   pNODE pn;
   if((pn = (pNODE)malloc(sizeof(NODE))) == NULL)
      return NULL;

   pn->left = pn->right = NULL;
   pn->val = val;
   pn->label = 0;
   pn->label_len = 0;
   return pn;
}

QUEUE new_queue()
{
   QUEUE que;
   que.first = que.last = NULL;
   que.nelem = 0;
   return que;
}

int get_nelem(qpNODE node)
{
   return node->elem->nelem;
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
      if(cmp_val <= get_nelem(que->first))
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
      if(cmp_val <= get_nelem(que->first)) // wstaw na początek
      {
         qpn->next = que->first;
         que->first = qpn;
      }
      else
      {
         qpNODE act = que->first;
         while(act != que->last && cmp_val > get_nelem(act->next))
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

void label_huffman_tree_rec(pNODE root, int path_len, int label, int set)
{
   root->label = label;
   // nadaj etykietę węzłowi drzewa
   root->label |= (set << (INT_BITS - path_len - 1));

   if(root->left == NULL && root->right == NULL)
   {
      // aktualizuje ilość bitów użytych w etykiecie
      // dotyczy tylko liści, ponieważ to one
      // są poszczególnymi literami
      root->label_len = path_len+1;
      return;
   }
   else
   {
      if(root->left != NULL)
         label_huffman_tree_rec(root->left, path_len+1, root->label, 0);
      if(root->right != NULL)
         label_huffman_tree_rec(root->right, path_len+1, root->label, 1);
   }
}

void label_huffman_tree(pNODE root)
{
   if(! root)
      return;

   if(root->left)
      label_huffman_tree_rec(root->left, 0, 0, 0);
   if(root->right)
      label_huffman_tree_rec(root->right, 0, 0, 1);
}

pNODE create_huffman_tree(unsigned long long *letter_cnt)
{
   int j;
   pNODE node_tab[UCHAR_MAX+1];
   QUEUE que = new_queue();

   for(int i = 0; i < UCHAR_MAX+1; ++i)
      node_tab[i] = NULL;

   for(j = 0; j < UCHAR_MAX+1; ++j) {
      if(letter_cnt[j]) // jeśli jakiś znak wystąpił przynajmniej raz
      {
         node_tab[j] = make_node((char)j);
         node_tab[j]->val   = (char)j;
         node_tab[j]->nelem = letter_cnt[j];
      }
   }

   for(int i = 0; i < j; ++i)
      add_elem_with_sort(&que, node_tab[i]);

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

   label_huffman_tree(root); // etykietuj drzewo

//   print_queue(&que);
   return root;
}

void write_header(unsigned long long tab[], FILE *outFile)
{
   int cnt_signs = 0;
   // liczy ile znaków użytych było w pliku
   for(int i = 0; i < UCHAR_MAX+1; ++i)
      if(tab[i])
         ++cnt_signs;

   // zapisz liczbę znaków
   fwrite(&cnt_signs, sizeof(int), 1, outFile);
   for(int i = 0; i < UCHAR_MAX+1; ++i)
      if(tab[i])
      {
         // zapisz znak
         fwrite(&i, sizeof(int), 1, outFile);
         // zapisz częstotliwość
         fwrite(&tab[i], sizeof(unsigned long long), 1, outFile);
      }
}

pNODE find_char_node(int ch, pNODE *map)
{
   return map[ch];
}

void map_tree_rek(pNODE node, pNODE *tab)
{
   if(node->left == NULL && node->right == NULL)
   {
      tab[node->val] = node;
      return;
   }
   if(node->left)
      map_tree_rek(node->left, tab);
   if(node->right)
      map_tree_rek(node->right, tab);
}

pNODE *map_tree(pNODE tree)
{
   pNODE *table = (pNODE*)malloc((UCHAR_MAX+1)*sizeof(pNODE));
   for(int i = 0; i < UCHAR_MAX+1; ++i)
      table[i] = NULL;

   map_tree_rek(tree, table);
   return table;
}

void huffman_encode(FILE *inFile, FILE *outFile)
{
   unsigned long long letter_cnt[UCHAR_MAX+1]; // ostatnim znakiem będzie EOF
   pNODE hTree, pn;
   pNODE *map;
   int ch;
   unsigned long long nbits;

   // skojarz bfp z plikiem outFile
   bit_file_t *bfp = BitFile_make_new(outFile, WRITE);

   for(int i = 0; i < UCHAR_MAX+1; ++i) // zeruj liczniki
      letter_cnt[i] = 0;

   while((ch = fgetc(inFile)) != EOF) // zlicz znaki
      ++letter_cnt[ch];

   // tworzy drzewo Huffmana
   if((hTree = create_huffman_tree(letter_cnt)) == NULL)
   {
      fprintf(stderr, "nie udało się stworzyć drzewa huffmana\n");
      return;
   }

   fseek(inFile, 0L, SEEK_SET); // przewiń na początek pliku

   write_header(letter_cnt, outFile); // zapisz nagłówek
   map = map_tree(hTree); // mapuj drzewo na tablicę

   while((ch = fgetc(inFile)) != EOF)
   {
      pn = find_char_node(ch, map);
      if(pn == NULL)
      {
         perror("nie znaleziono znaku");
         continue;
      }
      if(pn == hTree)
         nbits = 1;
      else
         nbits = pn->label_len;

      for(unsigned int i = 0; i < nbits; ++i)
         BitFile_write_bit(CHECK_BIT(pn->label, INT_BITS-1-i), bfp);
   }

   // flush
   BitFile_flush(bfp);
   return;
}

void huffman_decode(FILE *inFile, FILE *outFile)
{
   unsigned long long letter_cnt[UCHAR_MAX+1], tmp_size; // ostatnim znakiem będzie EOF
   pNODE hTree;
   int ch, nlet;

   for(int i = 0; i < UCHAR_MAX+1; ++i) // zeruj liczniki
      letter_cnt[i] = 0;

   // wczytaj ilość liter
   fread(&nlet, sizeof(int), 1, inFile);
   while(nlet--)
   {
      fread(&ch, sizeof(int), 1, inFile);
      fread(&tmp_size, sizeof(unsigned long long), 1, inFile);
      letter_cnt[ch] = tmp_size;
   }

   // tworzy drzewo Huffmana
   if((hTree = create_huffman_tree(letter_cnt)) == NULL)
   {
      fprintf(stderr, "nie udało się stworzyć drzewa huffmana\n");
      return;
   }

   // skojarz bfp z plikiem inFile
   bit_file_t *bfp = BitFile_make_new(inFile, READ);

   pNODE cur = hTree;
   // na podstawie drzewa Huffmana i odczytywanych bitów znajduje odpowiednie znaki
   while((ch = BitFile_read_bit(bfp)) != EOF)
   {
      if(ch == 0)
         cur = cur->left;
      else cur = cur->right;

      if(cur->left == NULL && cur->right == NULL)
      {
         if(cur->nelem > 0) {
            fputc(cur->val, outFile);
            cur->nelem--;
         }

         cur = hTree;
         continue;
      }
   }
}
