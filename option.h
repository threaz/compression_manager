#ifndef OPTION_H
#define OPTION_H

enum options {
    CREATE, ADD, EXTRACT, COMPRESS, ERROR
};

#define MAX_NAME 1000
#define MAX_FILES 100

void analyze_options(int argc, char **argv);
int make_action();

#endif // OPTION_H
