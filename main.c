#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "option.h"

int main(int argc, char *argv[])
{
    // analizuj opcje
    analyze_options(argc, argv);
    make_action();


    return 0;
}
