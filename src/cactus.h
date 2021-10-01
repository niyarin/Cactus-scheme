#ifndef CACTUS_H
#define CACTUS_H

#include<stdint.h>

typedef struct scm_object_t{
    uint8_t type;
    uintptr_t value;
} *scm_object;

typedef struct pair_cell_t{
    void* car;
    void* cdr;
} *pair_cell;

#define TYPE_PAIR 0
#define TYPE_FIX_NUM 1
#define TYPE_SYMBOL 2

scm_object make_scm_object(char type, uintptr_t ptr);
scm_object make_fixnum(intptr_t c_int);

#endif
