#include<stdlib.h>
#include<assert.h>
#include "cactus.h"

scm_object make_pair(cactus_runtime_controller controller, scm_object car, scm_object cdr){
    pair_cell cell = (pair_cell)malloc(sizeof(struct pair_cell_t));
    if (!cell){
        exit(1);
    }
    cell->car = car;
    cell->cdr = cdr;
    return make_scm_object(controller, TYPE_PAIR, (uintptr_t)cell);
}

scm_object make_list2(cactus_runtime_controller controller,
                      ScmObject obj1,
                      ScmObject obj2){
    return make_pair(controller, obj1, make_pair(controller, obj2, null_object));
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

ScmObject assq(ScmObject key, ScmList alist){
    ScmList cell = alist;
    while (!null_p(cell)){
        if (ref_car(ref_car(cell)) == key){
            return ref_car(cell);
        }
        cell = ref_cdr(cell);
    }
    return false_object;
}

ScmObject memq(ScmObject object, ScmList ls){
  ScmList l = ls;
  while (!null_p(l)){
    if (ref_car(l) == object){
      return l;
    }
  }
  return false_object;
}
