#ifndef METADATA_H
#define METADATA_H

#define META_MAX_NAME 1000

struct Metadata {
    char Name[META_MAX_NAME];
    unsigned int Size;
};

typedef struct Metadata Meta;

#endif // METADATA_H
