#include<string.h>
#include<assert.h>
#include "cactus.h"

scm_object lookup(cactus_runtime_controller controller, scm_symbol var){
    scm_object apair = assq(var, controller->global);
    if (apair !=  false_object){
        return ref_cdr(ref_car(apair));
    }else{
        //lookup local
        assert(0);
    }
}

void add_global(cactus_runtime_controller controller,
                scm_symbol var,
                scm_object val){
    controller->global = make_pair(controller, make_pair(controller,var,val),
                                               controller->global);
}

void add_global_syntax(cactus_runtime_controller controller,
                       scm_symbol var,
                       scm_object val){

    assert(pair_p(controller->macro_env));

    scm_object global_macro_env = controller->macro_env;
    while (!null_p(ref_cdr(global_macro_env))){
        global_macro_env = ref_cdr(global_macro_env);
    }

    set_car(global_macro_env, make_pair(controller,make_pair(controller, var, val),
                                                   ref_car(global_macro_env)));
}

scm_syntax lookup_syntax(cactus_runtime_controller controller,scm_symbol var, scm_object val){
    scm_pair syntax_stack_cell = controller->macro_env;
    while (null_p(syntax_stack_cell)){
        scm_object apair = assq(var, ref_car(syntax_stack_cell));
        if (apair != false_object){
            return ref_car(apair);
        }
        syntax_stack_cell = ref_cdr(syntax_stack_cell);
    }
    return null_object;
}
