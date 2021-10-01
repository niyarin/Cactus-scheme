#include<stdlib.h>
#include "cactus.h"

scm_object make_pair(scm_object car, scm_object cdr){
    pair_cell cell = (pair_cell)malloc(sizeof(struct pair_cell_t));
    cell->car = car;
    cell->cdr = cdr;
    return make_scm_object(TYPE_PAIR, (uintptr_t)cell);
}

scm_object ref_car(scm_object pair){
    return ((pair_cell)pair->value)->car;
}

scm_object ref_cdr(scm_object pair){
    return ((pair_cell)pair->value)->cdr;
}

void set_car(scm_object pair, scm_object obj){
    ((pair_cell)pair->value)->car = obj;
}

void set_cdr(scm_object pair, scm_object obj){
    ((pair_cell)pair->value)->cdr = obj;
}
