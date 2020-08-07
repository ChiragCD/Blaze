#include <stdlib.h>
#include <string.h>

#include "namespace.h"

int hash(char * str) {
    int i = 0, h = 0;
    while(str[i]) {
        h += str[i];
        h %= 64;
        i++;
    }
    return h;
}

func * get_callable (namespace * n, char * str, int num_references, int num_values, int * is_global) {
    if(is_global) {
        if(n->outer) *is_global = 0;
        else *is_global = 1;
    }

    int bucket_num = hash(str);
    holder * h = n->buckets[bucket_num];
    func * f;
    while(h) {
        if(h->value->type != FUNCTION || strcmp(h->name, str)) {
            h = h->next;
            continue;
        }
        f = h->value->function;
        if(num_references != f->num_references || num_values != f->num_values) {
            h = h->next;
            continue;
        }
        return h->value->function;
    }
    if(n->outer) f = get_callable(n->outer, str, num_references, num_values, is_global);
    else f = NULL;
    return f;
}

citizen * get_citizen (namespace * n, char * str, int * is_global) {
    if(is_global) {
        if(n->outer) *is_global = 0;
        else *is_global = 1;
    }

    int bucket_num = hash(str);
    holder * h = n->buckets[bucket_num];
    while(h) {
        if(strcmp(h->name, str) == 0) return h->value;
        h = h->next;
    }
    citizen * cz;
    if(n->outer) cz = get_citizen(n->outer, str, is_global);
    else cz = NULL;
    return cz;
}

void del_citizen (namespace * n, citizen * cz) {
    int bucket_num = hash(cz->name);
    holder * h = n->buckets[bucket_num];
    while(h) {
        if(h->value != cz) {
            h = h->next;
            continue;
        }
        if(h->prev == (*(n->buckets) + bucket_num)) n->buckets[bucket_num] = h->next;
        else h->prev->next = h->next;
        if(h->next) h->next->prev = h->prev;
        free(h);
        n->num_elements--;
        return;
    }
    return;
}

void put_citizen (namespace * n, citizen * cz) {
    int bucket_num = hash(cz->name);
    holder * h = (holder *) malloc(sizeof(holder));
    h->value = cz;
    h->name = cz->name;
    h->prev = *(n->buckets) + bucket_num;
    h->next = n->buckets[bucket_num];
    if(h->next) h->next->prev = h;
    n->buckets[bucket_num] = h;
    n->num_elements++;
    return;
}

void purge_namespace (namespace * n) {
    for(int i = 0; i < 64; i++) {
        if(n->num_elements == 0) break;
        while(n->buckets[i]) {
            holder * h = n->buckets[i];
            n->buckets[i] = h->next;
            free_citizen(h->value);
            free(h);
        }
    }
    free(n);
}