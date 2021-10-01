#include<stdlib.h>
#include "cactus.h"

scm_object make_scm_object(char type, uintptr_t ptr){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res->value = ptr;
   return res;
}

scm_object make_fixnum(intptr_t c_int){
    return make_scm_object(TYPE_FIX_NUM, c_int);
}
