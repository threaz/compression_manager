#ifndef METADATA_H
#define METADATA_H

#define META_MAX_NAME 1000

typedef struct {
    char Name[META_MAX_NAME];
    unsigned int Size;
} Meta;

#endif // METADATA_H
