#include<stdlib.h>
#include "cactus.h"
#include<assert.h>

scm_object make_pair(scm_object car, scm_object cdr){
    pair_cell cell = (pair_cell)malloc(sizeof(struct pair_cell_t));
    cell->car = car;
    cell->cdr = cdr;
    return make_scm_object(TYPE_PAIR, (uintptr_t)cell);
}

scm_object ref_car(scm_object pair){
    assert(pair_p(pair));
    return ((pair_cell)pair->value)->car;
}

scm_object ref_cdr(scm_object pair){
    assert(pair_p(pair));
    return ((pair_cell)pair->value)->cdr;
}

void set_car(scm_object pair, scm_object obj){
    assert(pair_p(pair));
    ((pair_cell)pair->value)->car = obj;
}

void set_cdr(scm_object pair, scm_object obj){
    assert(pair_p(pair));
    ((pair_cell)pair->value)->cdr = obj;
}

int pair_p(scm_object object){
    return object->type == TYPE_PAIR;
}
