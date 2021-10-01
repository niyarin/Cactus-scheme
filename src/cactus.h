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

scm_object make_pair(scm_object car, scm_object cdr);
scm_object ref_car(scm_object pair);
scm_object ref_cdr(scm_object pair);
void set_car(scm_object pair, scm_object obj);
void set_cdr(scm_object pair, scm_object obj);

#endif
