#include "compression.h"

int copy_files(FILE *inFile, FILE *outFile)
{
   size_t total_copied, bytes;
   char buffer[2048];

   total_copied = bytes = 0;
   while((bytes = fread(buffer, 1, sizeof(buffer), inFile)) > 0) {
      fwrite(buffer, 1, bytes, outFile);
      total_copied += bytes;
   }

   return total_copied;
}

int compress_with_params(FILE *inFile, FILE *outFile, archive_data *params)
{
   if(inFile == NULL || outFile == NULL)
      return -1;
   if(params == NULL)
      return -2;

   if(params->compression == NONE)
      return ! copy_files(inFile, outFile);
   else if(params->compression == RLE)
      rle_encode(inFile, outFile);
   else if(params->compression == HUFF)
      huffman_encode(inFile, outFile);

   return 0;
}

int decompress_with_params(FILE *inFile, FILE *outFile, archive_data *params)
{
   if(inFile == NULL || outFile == NULL)
      return -1;
   if(params == NULL)
      return -2;

   if(params->compression == NONE)
      return ! copy_files(inFile, outFile);
   else if(params->compression == RLE)
      rle_decode(inFile, outFile);
   else if(params->compression == HUFF)
      huffman_decode(inFile, outFile);

   return 0;
}

