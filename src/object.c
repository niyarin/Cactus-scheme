#include<stdlib.h>
#include "cactus.h"

struct scm_object_t null_entity = {TYPE_NULL, 1, 0};
scm_object null_object = (scm_object)&null_entity;

scm_object make_scm_object(char type, uintptr_t ptr){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res-> value_is_not_reference = 0;
   res->value = ptr;
   return res;
}

scm_object make_const_scm_object(char type, uintptr_t value){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res-> value_is_not_reference = 1;
   res->value = value;
   return res;
}

scm_object make_fixnum(intptr_t c_int){
    return make_const_scm_object(TYPE_FIX_NUM, c_int);
}

int fixnum_p(scm_object object){
    return object->type == TYPE_FIX_NUM;
}

scm_object make_primitive(primitive_procedure fn){
    return make_const_scm_object(TYPE_PRIMITIVE, (uintptr_t)fn);
}

int primitive_p(scm_object object){
    return ref_object_type(object) == TYPE_PRIMITIVE;
}

int null_p(scm_object object){
    return object == null_object;
}
