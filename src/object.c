#include<stdlib.h>
#include "cactus.h"

struct scm_object_t null_entity = {TYPE_NULL, 0};
scm_object null_object = (scm_object)&null_entity;

scm_object make_scm_object(char type, uintptr_t ptr){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res->value = ptr;
   return res;
}

scm_object make_fixnum(intptr_t c_int){
    return make_scm_object(TYPE_FIX_NUM, c_int);
}

int fixnum_p(scm_object object){
    return object->type == TYPE_FIX_NUM;
}
scm_object make_symbol(char* c_str){
    return make_scm_object(TYPE_SYMBOL, (uintptr_t)c_str);
}

int symbol_p(scm_object object){
    return object->type == TYPE_SYMBOL;
}

int null_p(scm_object object){
    return object == null_object;
}
