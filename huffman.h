#ifndef HUFFMAN_H
#define HUFFMAN_H

/*
 * Moduł zawiera funkcje pozwalające przeprowadzić kompresję plików
 * za pomocą kodowania Huffmana. Zaimplementowane są również struktury
 * danych stworzone specjalnie na potrzeby działania algorytmu.
 *
 * Algorytm opiera się na obserwacji, że gdy do reprezentacji znaków występujących
 * w tekście często użyjemy mniejszej liczby bitów, niż do reprezentacji znaków
 * występujących rzadko, w istocie zaoszczędzimy na tym cenną pamięć.
 *
 * Na początku zliczane są poszczególne znaki w tekście, a następnie utworzone zostają
 * elementy drzewa, które okażą się niedługo jego liśćmi. Zostaje utworzona kolejka,
 * w której umieszczane są najpierw elementy o najmniejszej liczbie wystąpień. Następnie
 * zdejmowane z niej są dwa elementy o najniższej takiej liczbie. Tworzony jest nowy węzeł,
 * który jako liczbę elementów zawiera sumę liczby elementów, z dwóch poprzednich. Jako lewe
 * i prawe poddrzewo ustawiamy wcześniejsze elementy.
 * Dodajemy tak utworzony korzeń w odpowiednie miejsce kolejki(dodawanie z zachowaniem porządku).
 * Wykonujemy te czynności aż w kolejce zostanie tylko jeden element, który zdejmujemy jako
 * korzeń drzewa Huffmana. Kolejnym krokiem jest etykietowanie drzewa, gdzie poszczególne znaki
 * otrzymają wreszcie unikalne ciągi bitów, którymi będą następnie kodowane. Następnie mapujemy
 * drzewo na tablicę dla szybszego dostępu, zapisujemy do pliku nagłówek, który jest niezbędny do
 * odbudowy drzewa oraz wpisujem do pliku wynikowego zamiast znaków, ich odpowiednie kody Huffmana.
 *
 * Dekompresja polega na odczytaniu nagłówka i na jego podstawie odtworzeniu drzewa Huffmana.
 * Następnie czytamy po jednym bicie z wyjścia(biblioteka BitFile) i przechodzimy po drzewie:
 * jeżeli 0-w lewo, jeżeli 1-w prawo. Gdy dojdziemy do liścia, przechowywana tam wartość jest
 * zakodowanym znakiem. Wypisujemy go do pliku wynikowego.
 */

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bitfile.h"

#define INT_BITS (sizeof(int)*8) // liczba bitów w int

/*
 * W kodowaniu Huffmana używa się drzewa, zwanego drzewem Huffmana.
 * Przechowuje ono w swoich liściach prawdopodobieństwo(w moim przypadku ilość)
 * wystąpienia poszczególnych znaków w tekście oraz sam znak. Dodatkowo zawiera
 * etykietę, na którą składają się ciagi bitów, którymi znaki będą kodowane.
 *
 * Jeżeli nie jesteśmy aktualnie w liściu, tylko innym węźle, nie pracujemy z innymi
 * polami tej struktury, jak nelem, left czy right.
 */
typedef struct NODE *pNODE;
typedef struct NODE
{
   unsigned char  val;       // znak
   unsigned long long nelem; // ilość wystąpień w tekście
   int label;                // etykieta
   int label_len;            // długość etykiety
   pNODE left;               // wskaźnik na lewe poddrzewo
   pNODE right;              // wskaźnik na prawe poddrzewo
} NODE;

// IMPLEMENTACJA KOLEJKI

/*
 * Do stworzenia drzewa Huffmana niezbędna jest kolejka, której elementy w moim
 * programie niejako "opakowują" elementy drzewa, na potrzeby przechowywania ich
 * w kolejce. Podczas wstawiania i zdejmowania elementów odpowiednio dołączmy
 * i zabieramy z kolejki przyszłe liście. Przez kolejne funkcje zostaną właściwie "opakowane".
 */

// element kolejki
typedef struct qNODE *qpNODE;
typedef struct qNODE
{
   pNODE elem;   // przechowywany element
   qpNODE next;  // wskaźnik na kolejny element kolejki
} qNODE;

// sama kolejka
typedef struct QUEUE
{
   qpNODE first; // głowa
   qpNODE last;  // ostatni element
   int nelem;    // ilość elem. w kolejce
} QUEUE;

/*
 * Tworzy nowy liść element drzewa, w szczególności tworzymy w programie liście.
 *
 * Parametry:
 *    val: znak, który chcemy przechowywać w liściu
 *
 * Zwraca:
 *    poprawnie zainicjalizowana struktura reprezentująca element drzewa
 */
pNODE make_node(char val);

/*
 * Tworzy nową kolejkę, przechowującą elementy z drzewa.
 *
 * Zwraca:
 *    nowa kolejka
 */
QUEUE new_queue();

/*
 * Zwraca ilość wystąpień znaku, "zapakowanego" aktualnie jako element kolejki.
 *
 * Parametry:
 *    node: element kolejki (nie mylić z elementem drzewa, który siedzi w tej paczuszce)
 *
 * Zwraca:
 *    int: ilość wystąpień przechowywanego w node znaku
 */
int get_nelem(qpNODE node);

/*
 * Dodaje element, będący jednym z węzłów drzewa Huffmana, do kolejki.
 *
 * Parametry:
 *    que: kolejka, na której koniec zostanie dodany element
 *    elem: element do dodania
 * Zwraca:
 *     0: sukces
 *    -1: wystapił błąd
 */
int add_elem(QUEUE *que, pNODE elem);

/*
 * Dodaje element, w kolejności niemalejącej, będący jednym z węzłów drzewa Huffmana,
 * do kolejki. (porównywanie odbywa się na podstawie pola nelem struktury elem).
 *
 * Parametry:
 *    que: kolejka, do której zostanie dodany element
 *    elem: element do dodania
 * Zwraca:
 *     0: sukces
 *    -1: wystapił błąd
 */
int add_elem_with_sort(QUEUE *que, pNODE elem);

/*
 * Usuwa pierwszy element z kolejki, zwracając "opakowywaną" do tej pory zawartość.
 *
 * Parametry:
 *    que: kolejka, z której zostanie usunięty pierwszy element
 *
 * Zwraca:
 *    wskaźnik na usunięty element
 *    NULL: w p.p.
 */
pNODE remove_elem(QUEUE *que);

/*
 * Mapuje drzewo Huffmana na tablicę wskaźników na wskaźniki do elementów drzewa.
 * Ułatwia to znacznie dostęp do poszczególnych znaków, w niej przechowywanych,
 * zapewniając dostęp w stałym czasie.
 *
 * Parametry:
 *    tree: drzewo do zmapowania
 *
 * Zwraca:
 *    tablica o UCHAR_MAX+1 elementach, pojemna na tyle aby zmieścić każdy element
 *    drzewa Huffmana
 */
pNODE *map_tree(pNODE tree);

/*
 * Pomocnicza funkcja, która rekurencyjnie przechodzi po całym drzewie Huffmana,
 * inicjalizując jednocześnie poszczególne komórki tablicy. Wywoływana pzez map_tree().
 *
 * Parametry:
 *    node: aktualnie sprawdzany element drzewa
 *    tab: tablica, która jest sukcesywnie uzupełniana
 */
void map_tree_rek(pNODE node, pNODE *tab);

/*
 * Szuka elementu drzewa o wskazanym znaku, w tablicy będącej zmapowanym drzewem.
 *
 * Parametry:
 *    ch: szukany znak
 *    map: tablica
 *
 * Zwraca:
 *    wskaznik do znalezionego elementu
 *    NULL: w p.p.
 */
pNODE find_char_node(int ch, pNODE *map);

/*
 * Tworzy drzewo Huffmana na podstawie tablicy zawierającej liczniki poszczególnych znaków.
 *
 * Parametry:
 *    letter_cnt: tablica z licznikami znaków, pod indeksem i-tym znajduje się znak
 *    o zapisie dziesiętnym wg ASCII - i.
 */
pNODE create_huffman_tree(unsigned long long *letter_cnt);

/*
 * Etykietuje drzewo Huffmana ze względu na jego strukturę, powstałą przy jego tworzeniu.
 * Elementy o największej liczbie wystąpień, zgodnie z założeniami algorytmu otrzymają
 * najkrótsze etykiety.
 *
 * Parametry:
 *    root: wskaznik na korzen drzewa
 */
void label_huffman_tree(pNODE root);

/*
 * Funkcja pomocnicza, która faktycznie przechodzi się po całym drzewie Huffmana, etykietuje
 * poszczególne węzły.
 *
 * Parametry:
 *    root: aktualnie badany węzeł
 *    path_len: dotychczasowa długość drogi = długość reprezentacji bitowej etykiety
 *    label: etykieta
 *    set: wskazuje, czy ustawić bit na 0(set=0) czy na 1(set!=0)
 */
void label_huffman_tree_rec(pNODE root, int path_len, int label, int set);

/*
 * Zapisuje specjalny nagłówek do pliku, aby możliwa była, podczas dekompresji, rekonstrukcja
 * drzewa Huffmana. Składa się ona z poszególnych, możliwych do wystąpienia znaków, a następnie
 * licznika wystąpień każdego z nich.
 *
 * Parametry:
 *    tab: tablica zawierająca liczniki poszczególnych znaków
 *    outFile: plik, do którego zostanie zapisany nagłówek
 */
void write_header(unsigned long long tab[], FILE *outFile);

/*
 * Kompresujeplik wejściowy za pomocą algorytmu kodowania Huffmana, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void huffman_encode(FILE *inFile, FILE *outFile);

/*
 * Dekompresuje plik wejściowy za pomocą kodowania Huffmana, wynik pracy zapisywany jest
 * w pliku wyjściowym.
 *
 * Parametry:
 *    inFile: plik wejściowy
 *    outFile: plik wyjściowy
 */
void huffman_decode(FILE *inFile, FILE *outFile);

#endif // HUFFMAN_H
