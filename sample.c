//#include "huffman.h"
//#include "lz77.h"
//#include "bitfile.h"

//int main()
//{
//   FILE *test = fopen("test", "rb");
//   bit_file_t *bfp;

//   bfp = BitFile_make_new(test, READ);

//   for(int i = 0; i < 4; ++i)
//      BitFile_write_bit(1, bfp);
//   BitFile_write_char('A', bfp);

//   for(int i = 0; i < 4; ++i)
//      BitFile_write_bit(1, bfp);
//   BitFile_write_char('B', bfp);

//   for(int i = 0; i < 4; ++i)
//      BitFile_write_bit(1, bfp);
//   BitFile_write_char('C', bfp);

//   BitFile_flush(bfp);

//   for(int i = 0; i < 3; ++i)
//   {
//      unsigned char uch = BitFile_read_nbits(4, bfp);
//      printf("%d liczba, ", uch);
//      uch = BitFile_read_nbits(8, bfp);
//      printf("%c litera\n", uch);
//   }
//   return 0;
//}
