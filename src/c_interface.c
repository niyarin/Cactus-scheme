#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include "cactus.h"

scm_object call_scm_primitive(cactus_runtime_controller controller, scm_primitive primitive, int n_args, ...){
    assert(primitive_p(primitive));
    va_list ap;
    va_start(ap, n_args);
    scm_object* arg_array = (scm_object*)malloc(sizeof(scm_object) * n_args);
    if (!arg_array){
        exit(1);
    }

    for (int i=0;i<n_args;i++){
        arg_array[i] = va_arg(ap, scm_object);
    }
    va_end(ap);

    primitive_procedure pp = (primitive_procedure)ref_object_value(primitive);

    scm_object res =  pp(controller, n_args, arg_array);
    free(arg_array);
    return res;
}

scm_object cact_cons(cactus_runtime_controller controller, int n_args,scm_object *arg_array){
    assert(n_args == 2);
    return make_pair(controller, arg_array[0], arg_array[1]);
}

scm_object cact_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array){
    assert(n_args == 1);
    return ref_car(arg_array[0]);
}

scm_object cact_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array){
    assert(n_args == 1);
    return ref_cdr(arg_array[0]);
}

scm_object cact_set_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array){
    assert(n_args == 2);
    set_car(arg_array[0], arg_array[1]);
    return null_object;
}

scm_object cact_set_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array){
    assert(n_args == 2);
    set_cdr(arg_array[0], arg_array[1]);
    return null_object;
}

scm_object cact_values(cactus_runtime_controller controller,int n_args,scm_object *args_array){
    assert(n_args >= 1);

    controller->n_values = n_args-1;
    controller->values = (scm_object)malloc(sizeof(scm_object) * (n_args-1));
    for (int i=1;i<n_args;i++){

    }

    return args_array[0];
}
