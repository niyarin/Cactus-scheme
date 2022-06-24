#include<string.h>
#include<assert.h>
#include "cactus.h"

static ScmObject lookup_local(cactus_runtime_controller controller, scm_symbol var){
    ScmObject local_stack = controller->local_stack;
    while (!null_p(local_stack)){
        ScmObject found = assq(var, ref_car(local_stack));
        if (found != false_object){
            return found;
        }
        local_stack = ref_cdr(local_stack);
    }
    return false_object;
}

static ScmObject lookup_cell(cactus_runtime_controller controller, scm_symbol var){
    assert(symbol_p(var));
    ScmObject apair = lookup_local(controller, var);
    if (apair != false_object){
        return apair;
    }

    apair = assq(var, controller->global);
    if (apair !=  false_object){
        return apair;
    }

    {//REMOVE LATOR
        printf("\n\n");
        simple_write(stdout, var);
        printf("\n\n");
        assert(0);//fail
    }
}

ScmObject lookup(cactus_runtime_controller controller, ScmObject var){
    if (identifier_p(var)){
        var = ref_identifier_symbol(var);
    }else{
        assert(symbol_p(var));
    }
    return ref_cdr(lookup_cell(controller, var));
}

void update(cactus_runtime_controller controller, scm_symbol var, ScmObject val){
    assert(symbol_p(var));
    ScmObject apair = lookup_cell(controller, var);
    set_cdr(apair, val);
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

    scm_symbol syntax_symbol = make_const_symbol(controller, "syntax");
    symbol_intern(controller, syntax_symbol);

    scm_object global_macro_env = controller->macro_env;
    while (!null_p(ref_cdr(global_macro_env))){
        global_macro_env = ref_cdr(global_macro_env);
    }

    set_cdr(global_macro_env, make_pair(controller,make_pair(controller, var, make_pair(controller,  syntax_symbol, val)),
                                                   null_object));
}

scm_syntax lookup_syntax(ScmObject macro_env ,scm_symbol var){
    scm_pair syntax_stack_cell = macro_env;
    scm_object apair = assq(var, ref_car(syntax_stack_cell));
    if (apair != false_object){
        return ref_cdr(apair);
    }
    return null_object;
}
