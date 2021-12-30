#include<stdlib.h>
#include "cactus.h"

ScmObject make_char(cactus_runtime_controller controller, uint32_t u32_char){
    return make_const_scm_object(controller,  TYPE_CHAR,  u32_char);
}

ScmObject make_string(cactus_runtime_controller controller,char* cstr){
    return make_scm_object(controller, TYPE_STRING, (uintptr_t)cstr);
}

int string_p(scm_object object){
    return ref_object_type(object) == TYPE_STRING;
}

char* string_to_cstr(ScmObject object){
    return ref_object_value(object);
}
