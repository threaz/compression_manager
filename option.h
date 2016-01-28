#ifndef OPTION_H
#define OPTION_H

/*
 * Moduł ten jest silnie zintegrowany z tym konkretnym programem, ponieważ
 * istnieją w nim funkcje, które analizują opcje podane przez użytkownika.
 * Zostaną one następnie zastosowane do archiwum. Są tutaj również zdefiniowane
 * funkcje, które na podstawie udostępnionych im opcji, są w stanie przekazać
 * informację o tym, jakie metadane posiada archiwum oraz jakie akcje ma wykonać
 * program. Obydwie informacje przekazywane są w postaci dobrze zdefiniowanych
 * typów danych.
 */

#include "archive.h"
#include "compression.h"
#include <string.h>

/* Typ danych, w którym będą przechowywane opcje, które zostaną użyte w programie,
 * jak również dane o archiwum i plikach znajdujących/mających się w nim znajdować.
 */
typedef struct
{
   char ar_name_buf[MAX_NAME];  // przechowuje nazwę archiwum
   name_buffer  buf;            // bufor na nazwy plikow
   unsigned int nfiles;         // licznik plików
   unsigned int compression;    // rodzaj kompresji, domyślnie powinien być NONE

   int action;                  // wybrane działanie na archiwum
   int direction;               // definiuje kierunek, w którym zostanie podjęta akcja
} option_t;

/*
 * Tworzy strukturę do przechowywania opcji programu i informacji o archiwum
 *
 * Zwraca:
 *    nowa, zainicjalizowana struktura
 */
option_t init_option();

/*
 * Wyświetla na standardowe wyjście poprawne wywołanie programu oraz opisuje
 * pokrótce każdą z jego opcji
 */
void yield_usage();

/*
 * Wyłuskuje opcje podane przez użytkownika i segreguje je starannie, nadając wartości
 * poszczególnym polom struktury, przekazanej przez wskaźnik, jako argument.
 * Tak przygotowane dane, są potem o wiele łatwiejsze w dalszej obróbce.
 *
 * Parametry:
 *    argc: ilość opcji podanych przez użytkownika(liczy się tutaj nazwa programu)
 *    argv: wektor z opcjami, jako wskaźnik na napisy
 *    options: struktura, która zostanie zainicjalizowana w zależności od argc opcji z argv
 *
 * Zwraca:
 *     0: sukces
 *    -1: w p.p.
 */
int extract_options(int argc, char **argv, option_t *options);

/*
 * Wybiera z podanych opcji te, które bezpośrednio opisują archiwum i jego zawartość.
 * Zwracana struktura znacznie lepiej kojarzy się z przechowywanymi danymi.
 *
 * Parametry:
 *    options: opcje, wcześniej powinny być zainicjalizowane przez użycie extract_options
 *
 * Zwraca:
 *    struktura z odpowiednio zainicjalizowanymi polami, opisuje archiwum i jego zawartość
 */
archive_data get_archive_data(option_t options);

/*
 * Wybiera z podanych opcji te, które bezpośrednio opisują akcje, które zostaną wykonane
 * w programie.
 *
 * Parametry:
 *    options: opcje, wcześniej powinny być zainicjalizowane przez użycie extract_options
 *
 * Zwraca:
 *    struktura z odpowiednio zainicjalizowanymi polami, opisuje akcje podejmowane w programie
 */
action_t     get_archive_action(option_t options);

#endif // OPTION_H
