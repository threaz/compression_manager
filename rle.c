#include "rle.h"

void rle_encode(FILE *inFile, FILE *outFile)
{
    char ch, p_ch;
    unsigned char cnt;

    cnt = 0;
    fread(&ch, sizeof(char), 1, inFile);
    p_ch = ch + 1; // p_ch musi się różnić od ch
    while(! feof(inFile))
    {
        if(p_ch == ch)
        {
            // maksymalna długość serii, którą można zakodować
            // została osiągnięta
            if(cnt == 255)
            {
                fwrite(&ch, sizeof(char), 1, outFile);
                fwrite(&cnt, sizeof(char), 1, outFile);
                // ustaw stan początkowy
                cnt = 0;
                p_ch = ch + 1;
                fread(&ch, sizeof(char), 1, inFile);
                continue;
            }
            else ++cnt;
        }
        else
        {
            if(cnt == 0) // nie ma pozostałości do wyświetlenia
                fwrite(&ch, sizeof(char), 1, outFile);
            else
            {
                --cnt;
                // wpisz pozostałe razem z licznikiem
                fwrite(&p_ch, sizeof(char), 1, outFile);
                fwrite(&cnt, sizeof(char), 1, outFile);
                // wpisz też nowy znak
                fwrite(&ch, sizeof(char), 1, outFile);
                cnt = 0;
            }
        }
        p_ch = ch;
        fread(&ch, sizeof(char), 1, inFile);
    }
    // wypisz koncówkę
    if(cnt != 0)
    {
        --cnt;
        // wpisz pozostałe razem z licznikiem
        fwrite(&p_ch, sizeof(char), 1, outFile);
        fwrite(&cnt, sizeof(char), 1, outFile);
    }

}

void rle_decode(FILE *inFile, FILE *outFile)
{
    char ch, p_ch;
    unsigned char cnt;

    fread(&ch, sizeof(char), 1, inFile);
    p_ch = ch + 1; // p_ch musi być różne od ch
    while(! feof(inFile))
    {
        if(p_ch == ch) // wystąpiła podwójna litera
        {
            // wypisz drugą literę
            fwrite(&ch, sizeof(char), 1, outFile);
            // wczytaj ilość powtórzeń
            fread(&cnt, sizeof(char), 1, inFile);
            while(cnt--) // wypisz wszystkie litery
                fwrite(&ch, sizeof(char), 1, outFile);

            // powrót do stanu początkowego
            fread(&ch, sizeof(char), 1, inFile);
            p_ch = ch + 1;
            continue;
        }
        else
            fwrite(&ch, sizeof(char), 1, outFile);

        p_ch = ch;
        fread(&ch, sizeof(char), 1, inFile);
    }
}
