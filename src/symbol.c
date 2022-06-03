#include<string.h>
#include<assert.h>
#include "cactus.h"

ScmSymbol make_symbol(cactus_runtime_controller controller, char* c_str){
    return make_scm_object(controller, TYPE_SYMBOL, (uintptr_t)c_str);
}

ScmSymbol make_const_symbol(cactus_runtime_controller controller, char* c_str){
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

ScmObject search_intern_box(scm_symbol symbol, scm_pair intern_box){
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
    return null_object;
}

ScmSymbol symbol_intern(cactus_runtime_controller controller, ScmSymbol symbol){
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

ScmSymbol rename_symbol(cactus_runtime_controller controller, ScmSymbol sym){
    char *str = (char*)ref_object_value(sym);
    size_t slen = strlen(str) ;
    char *new_str = (char*)malloc(sizeof(char) * (slen + 1));
    strcpy(new_str, str);
    new_str[slen+1] = '\0';

    ScmSymbol res = make_symbol(controller, new_str);
    for (int i=0;i<10;i++){
        new_str[slen] = '0' + i;

        ScmObject interned = search_intern_box(res, controller->symbol_intern);
        if (null_p(interned)){
            return res;
        }
    }
    new_str[slen] = '*';
    return rename_symbol(controller, res);
}
