#include "bitfile.h"

bit_file_t *BitFile_make_new(FILE *stream, BF_MODE mode)
{
   if(stream == NULL)
      return NULL;

   bit_file_t *bfp;

   if((bfp = (bit_file_t*)malloc(sizeof(bit_file_t))) == NULL)
      return NULL;

   bfp->fp = stream;

   if(mode == READ)
      bfp->bitBuffer = fgetc(bfp->fp);
   else
      bfp->bitBuffer = 0;

   bfp->bitLeft   = 8;
   bfp->mode      = mode;

   return bfp;
}

int BitFile_write_bit(int bit, bit_file_t *bfp)
{
   if(bfp->bitLeft <= 0)
   {
      fputc(bfp->bitBuffer, bfp->fp);
      bfp->bitBuffer = 0;
      bfp->bitLeft = 8;
   }

   if(bit) // ustaw bit
      bfp->bitBuffer |= (1 << (bfp->bitLeft - 1));
   // w p.p. pozostaw jako zero
   bfp->bitLeft--;

   return 0;
}

int BitFile_read_bit(bit_file_t *bfp)
{
   if(bfp->bitLeft == 0)
   {
      int ch;
      ch = fgetc(bfp->fp);
      if(ch == EOF)
         return ch;

      bfp->bitBuffer = ch;
      bfp->bitLeft = 8;
   }

   int ret = bfp->bitBuffer & (1 << (bfp->bitLeft - 1));
   bfp->bitLeft--;

   return ret;
}
