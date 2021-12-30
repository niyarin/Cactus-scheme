#include "cactus.h"
#include<stdlib.h>

void boot_pair(cactus_runtime_controller controller){
    //cons
    scm_symbol cons_symbol = make_const_symbol(controller, "cons");
    symbol_intern(controller, cons_symbol);
    add_global(controller, cons_symbol, make_primitive(controller, &cact_cons));

    //car
    scm_symbol car_symbol = make_const_symbol(controller, "car");
    symbol_intern(controller, car_symbol);
    add_global(controller, car_symbol, make_primitive(controller, &cact_car));

    //cdr
    scm_symbol cdr_symbol = make_const_symbol(controller, "cdr");
    symbol_intern(controller, cdr_symbol);
    add_global(controller, cdr_symbol, make_primitive(controller, &cact_cdr));

    //set-car!
    scm_symbol set_car_symbol = make_const_symbol(controller, "set-car!");
    symbol_intern(controller, set_car_symbol);
    add_global(controller, set_car_symbol, make_primitive(controller, &cact_set_car));

    //set-cdr!
    scm_symbol set_cdr_symbol = make_const_symbol(controller, "set-cdr!");
    symbol_intern(controller, set_cdr_symbol);
    add_global(controller, set_cdr_symbol, make_primitive(controller, &cact_set_cdr));

    //pair?
    scm_symbol pair_p_symbol = make_const_symbol(controller, "pair?");
    symbol_intern(controller, pair_p_symbol);
    add_global(controller, pair_p_symbol, make_primitive(controller, &cact_pair_p));
}

void boot_vector(cactus_runtime_controller controller){
    //make-vector
    scm_symbol make_vector_symbol = make_const_symbol(controller, "make-vector");
    symbol_intern(controller, make_vector_symbol);
    add_global(controller, make_vector_symbol, make_primitive(controller, &cact_make_vector));
}

void boot_other_primitive(cactus_runtime_controller controller){
    //eq?
    scm_symbol eq_symbol = make_const_symbol(controller, "eq?");
    symbol_intern(controller, eq_symbol);
    add_global(controller, eq_symbol, make_primitive(controller, &cact_eq));
}

void boot_procedures(cactus_runtime_controller controller){
    /*
    //call/cc
    scm_symbol call_cc_symbol = make_const_symbol(controller, "call/cc");
    symbol_intern(controller, call_cc_symbol);
    add_global(controller, call_cc_symbol, make_primitive(controller, &cact_call_cc));
    */

    //call-with-values
    scm_symbol call_with_values_symbol = make_const_symbol(controller, "call-with-values");
    symbol_intern(controller, call_with_values_symbol);
    add_global(controller, call_with_values_symbol, make_primitive(controller, &cact_call_with_values));
}

void boot_writer(cactus_runtime_controller controller){
    scm_symbol write_char_symbol = make_const_symbol(controller, "write-char");
    symbol_intern(controller, write_char_symbol);
    add_global(controller, write_char_symbol, make_primitive(controller, &cact_write_char));
}

void boot_syntax(cactus_runtime_controller controller){
    //quote
    scm_symbol quote_symbol = make_const_symbol(controller, "quote");
    symbol_intern(controller, quote_symbol);
    add_global_syntax(controller, quote_symbol, syntax_quote_object);

    //lambda
    scm_symbol lambda_symbol = make_const_symbol(controller, "lambda");
    symbol_intern(controller, lambda_symbol);
    add_global_syntax(controller, lambda_symbol, syntax_lambda_object);

    //define
    scm_symbol define_symbol = make_const_symbol(controller, "define");
    symbol_intern(controller, define_symbol);
    add_global_syntax(controller, define_symbol, syntax_define_object);

    //if
    scm_symbol if_symbol = make_const_symbol(controller, "if");
    symbol_intern(controller, if_symbol);
    add_global_syntax(controller, if_symbol, syntax_if_object);

    //set!
    scm_symbol set_symbol = make_const_symbol(controller, "set!");
    symbol_intern(controller, set_symbol);
    add_global_syntax(controller, set_symbol, syntax_set_object);

    //call/cc%
    scm_symbol call_cc_internal_symbol = make_const_symbol(controller, "call/cc%");
    symbol_intern(controller, call_cc_internal_symbol);
    add_global_syntax(controller, call_cc_internal_symbol, syntax_call_cc_internal_object);

    //call-with-values
    scm_symbol call_with_values_internal_symbol = make_const_symbol(controller, "call-with-values%");
    symbol_intern(controller, call_with_values_internal_symbol);
    add_global_syntax(controller, call_with_values_internal_symbol, syntax_call_with_values_internal_object);
}


void boot_controller(cactus_runtime_controller controller){
    controller-> all_objects_area_size = 32;
    controller-> all_objects_size = 0;
    controller->all_objects = (scm_object*)malloc(sizeof(scm_object) * controller-> all_objects_area_size);
    if (controller->all_objects == NULL){
        exit(1);
    }
    scm_pair intern_box = make_pair(controller, null_object, null_object);
    controller->symbol_intern = intern_box;
    controller->gc_roots = null_object;

    controller->global = null_object;
    controller->macro_env = make_pair(controller,null_object,null_object);
    controller->local_stack = null_object;
}

void boot_load_path(){
    const char *env_var = getenv('CACTUS_LOAD_PATH');
    if (env_var != NULL) {
        printf("%s\n",env_var);
    }
    printf("LOAD PATH!\n");
}

void boot(cactus_runtime_controller controller){
    boot_controller(controller);
    boot_pair(controller);
    boot_procedures(controller);
    boot_other_primitive(controller);
    boot_syntax(controller);
    boot_writer(controller);

    boot_load_path();
}
