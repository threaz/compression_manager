#include "bitfile.h"

int BitFile_flush(bit_file_t *bfp)
{
   // jeżeli bufor jest pusty, nic nie rób
   if(bfp->bitLeft == UCHAR_BITS)
      return 1;

   // wypisz cały bufor
   fputc(bfp->bitBuffer, bfp->fp);
   // nadaj wartości początkowe licznikom
   bfp->bitBuffer = 0;
   bfp->bitLeft   = UCHAR_BITS;

   return 0;
}

bit_file_t *BitFile_make_new(FILE *stream, BF_MODE mode)
{
   if(stream == NULL)
      return NULL;

   bit_file_t *bfp;
   if((bfp = (bit_file_t*)malloc(sizeof(bit_file_t))) == NULL)
      return NULL;

   // skojarz bfp z odpowiednim strumieniem danych
   bfp->fp = stream;
   if(mode == READ)
      bfp->bitBuffer = fgetc(bfp->fp); // przeczytaj pierwszy znak z wejscia
   else // WRITE
      bfp->bitBuffer = 0; // dostajemy czysty bufor

   bfp->bitLeft   = UCHAR_BITS;
   bfp->mode      = mode;

   return bfp;
}

int BitFile_read_bit(bit_file_t *bfp)
{
   if(bfp->bitLeft == 0)
   {
      int ch;
      ch = fgetc(bfp->fp);

      if(ch == EOF)
         return ch;

      bfp->bitLeft = UCHAR_BITS;
      bfp->bitBuffer = ch;
   }

   int ret = bfp->bitBuffer & (1 << (bfp->bitLeft - 1));
   bfp->bitLeft--;

   return ret;
}

int BitFile_read_char(bit_file_t *bfp)
{
   // cały bufor jest zapełniony, zwróć go
   if(bfp->bitLeft == UCHAR_BITS)
   {
      int ch = bfp->bitBuffer;
      bfp->bitLeft = 0;

      return ch;
   }

   int ch;
   unsigned char uch, tmp;

   ch = fgetc(bfp->fp);
   if(ch == EOF)
      return ch;

   // gdy nic nie ma do zwrócenia, oddajemy kolejny znak z wejścia
   if(bfp->bitLeft == 0)
      return ch;

   // jezeli coś zalega w buforze
   bfp->bitBuffer <<= UCHAR_BITS - bfp->bitLeft;
   bfp->bitBuffer |= (ch >> bfp->bitLeft);

   // zapisz to, co mamy wypisać
   tmp = bfp->bitBuffer;

   bfp->bitBuffer = 0;
   uch = (ch << (UCHAR_BITS - bfp->bitLeft));
   uch >>= (UCHAR_BITS - bfp->bitLeft);
   bfp->bitBuffer = uch;

   return tmp;
}

int BitFile_write_bit(int bit, bit_file_t *bfp)
{
   if(bfp->bitLeft <= 0)
   {
      fputc(bfp->bitBuffer, bfp->fp);
      bfp->bitBuffer = 0;
      bfp->bitLeft = UCHAR_BITS;
   }

   if(bit) // ustaw bit
      bfp->bitBuffer |= (1 << (bfp->bitLeft - 1));
   // w p.p. pozostaw jako zero
   bfp->bitLeft--;

   return 0;
}

int BitFile_write_char(unsigned char ch, bit_file_t *bfp)
{
   if(bfp->bitLeft == 0) // bufor nadaje się do wypisania
   {
      fputc(bfp->bitBuffer, bfp->fp);
      bfp->bitBuffer = 0;
      bfp->bitLeft = UCHAR_BITS;
   }

   if(bfp->bitLeft == UCHAR_BITS) // można po prostu wypisać ch
      fputc(ch, bfp->fp);
   else // w buforze znajduja się już jakieś bity
   {
      int i;
      for(i = 1; i <= bfp->bitLeft; ++i) // jeżeli jeszcze jest miejsce, dopisz do bufora
      {
         int tmp = CHECK_BIT(ch, UCHAR_BITS - i) ? 1 : 0;
         bfp->bitBuffer |= (tmp << (bfp->bitLeft - i));
      }

      // bufor pełny -> wypisz bufor
      fputc(bfp->bitBuffer, bfp->fp);
      bfp->bitBuffer = 0;

      i--; // zawędrowaliśmy z i za daleko o 1

      // zapisz do bufora pozostałe bity z ch
      bfp->bitBuffer = (ch << bfp->bitLeft);
      // zaktualizuj licznik
      bfp->bitLeft =  bfp->bitLeft;
   }
   return 0;
}

int BitFile_read_nbits(int nbits, bit_file_t *bfp)
{
   int ch;
   unsigned char uch, tmp;

   // bufor jest pusty, dociągamy nowe znaki
   if(bfp->bitLeft == 0)
   {
      ch = fgetc(bfp->fp);
      if(ch == EOF)
         return 1;
      bfp->bitBuffer = ch;
      bfp->bitLeft   = UCHAR_BITS;
   }

   if(bfp->bitLeft < nbits)
   {
      // kopiuj bufor
      uch = bfp->bitBuffer;
      // usuń bity z lewej
      uch <<= (UCHAR_BITS - bfp->bitLeft);
      // dosuń je odpowiednio do prawej
      uch >>= (UCHAR_BITS - nbits);
      // wczytaj kolejne bity
      ch = fgetc(bfp->fp);
      // koniec pliku, wypisz to, co mamy
      if(ch == EOF)
      {
         bfp->bitLeft = 0;
         bfp->bitBuffer = 0;
         return uch;
      }

      tmp = ch;
      // dodaj nowe bity
      uch |= (tmp >> (UCHAR_BITS - nbits + bfp->bitLeft));
      // zaktualizuj bufor
      bfp->bitBuffer = tmp;
      // zaktualizuj licznik
      bfp->bitLeft = (UCHAR_BITS - nbits + bfp->bitLeft);
   }
   else // mamy wystarczająco bitów w buforze
   {
      // skopiuj bufor
      uch = bfp->bitBuffer;
      // skasuj zbędne bity z lewej strony
      uch <<= (UCHAR_BITS - bfp->bitLeft);
      // wróć z nimi na miejsce
      uch >>= (UCHAR_BITS - bfp->bitLeft);
      // wyrzuć niepotrzebne z prawej
      uch >>= (bfp->bitLeft - nbits);
      // aktualizuj licznik
      bfp->bitLeft = bfp->bitLeft - nbits;
   }
   return uch;
}
