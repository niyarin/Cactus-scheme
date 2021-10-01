#include<string.h>
#include<assert.h>
#include "cactus.h"

scm_object make_symbol(cactus_runtime_controller controller, char* c_str){
    return make_scm_object(controller, TYPE_SYMBOL, (uintptr_t)c_str);
}

scm_object make_const_symbol(cactus_runtime_controller controller, char* c_str){
    return make_const_scm_object(controller, TYPE_SYMBOL, (uintptr_t)c_str);
}

int symbol_p(scm_object object){
    return ref_object_type(object) == TYPE_SYMBOL;
}

int simple_symbol_eq(scm_non_interned_symbol symbol1,
                     scm_non_interned_symbol symbol2){
    assert(symbol_p(symbol1));
    assert(symbol_p(symbol2));
    if (symbol1 == symbol2){
        return 1;
    }
    char* symbol_value1 = (char*)ref_object_value(symbol1);
    char* symbol_value2 = (char*)ref_object_value(symbol2);
    return strcmp(symbol_value1, symbol_value2) == 0;
}

scm_object search_intern_box(scm_symbol symbol, scm_pair intern_box){
    assert(symbol_p(symbol));
    assert(pair_p(intern_box));
    scm_list ls = ref_car(intern_box),
             cell = ls;
    while (!null_p(cell)){
        if (simple_symbol_eq(ref_car(cell), symbol)){
            return ref_car(cell);
        }
        cell = ref_cdr(cell);
    }
    return cell;
}

scm_object symbol_intern(cactus_runtime_controller controller, scm_symbol symbol){
    assert(symbol_p(symbol));
    assert(pair_p(controller->symbol_intern));
    scm_list ls = ref_car(controller->symbol_intern);

    scm_object search_res = search_intern_box(symbol, controller->symbol_intern);
    if (null_p(search_res)){
        set_car(controller->symbol_intern, make_pair(controller, symbol, ls));
        return symbol;
    }
    return search_res;
}
