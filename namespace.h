#ifndef NAMESPACE_H
#define NAMESPACE_H

typedef enum identifier_type identifier_type;
typedef struct holder holder;
typedef struct namespace namespace;

#include "global.h"

struct holder {
    holder * next;
    holder * prev;
    char * name;
    citizen * value;
};

struct namespace {
    namespace * outer;
    int num_elements;
    holder * buckets[64];
    citizen ** keys;
};

func * get_callable(namespace * n, char * str, int num_references, int num_values, int * is_global);
citizen * get_citizen(namespace * n, char * str, int * is_global);
void put_citizen(namespace * n, citizen * cz);
void del_citizen(namespace * n, citizen * cz);
void purge_namespace(namespace * n);

#endif