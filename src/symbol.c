#include "cactus.h"

scm_object make_symbol(char* c_str){
    return make_scm_object(TYPE_SYMBOL, (uintptr_t)c_str);
}

int symbol_p(scm_object object){
    return object->type == TYPE_SYMBOL;
}
