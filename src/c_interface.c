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

// pair

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

ScmObject cact_eq(cactus_runtime_controller controller, int n_args,ScmObject *arg_array){
    assert(n_args == 2);
    if (arg_array[0] == arg_array[1]){
        return true_object;
    }else{
        return false_object;
    }
}

ScmObject cact_pair_p(cactus_runtime_controller controller, int n_args,ScmObject *arg_array){
    assert(n_args == 1);
    if (pair_p(arg_array[0])){
        return true_object;
    }else{
        return false_object;
    }
}

//vector
ScmObject cact_make_vector(cactus_runtime_controller controller, int n_args,ScmObject *arg_array){
    assert(0);
}

//procedure

static ScmObject apply_aux(cactus_runtime_controller controller, ScmObject procedure, ScmObject args, ScmObject tmp_eval_stack){
    ScmObject copied_eval_stack = controller->eval_stack;
    controller->eval_stack = tmp_eval_stack;
    ScmObject next = apply(controller, procedure, args);
    controller->evaled = null_object;
    ScmObject res = call_eval_step(controller,  next);
    controller->eval_stack = copied_eval_stack;
    return res;
}


ScmObject cact_call_cc(cactus_runtime_controller controller, int n_args,ScmObject *arg_array){
    //TCOの担保はできていない(中でcall/ccを更に呼びまくると,CのStackがあふれる)
    //この関数ではなく、applyで
    //外側から1度だけ呼び出されるなら問題なさそう(本当にCのinterface用)
    //インタプリタ用call/ccは、インタプリタ内部に突っ込む
    assert(n_args == 1);
    ScmObject continuation = make_continuation(controller);
    ScmObject procedure = arg_array[0];
    ScmObject next = apply(controller, procedure, make_pair(controller, continuation, null_object));
    return call_eval_step(controller,  next);
}

ScmObject cact_call_with_values(cactus_runtime_controller controller, int n_args, ScmObject *arg_array){
    ScmObject producer = arg_array[0];
    ScmObject consumer = arg_array[1];
    //外側では止まらない
    ScmObject tmp_eval_stack =
        make_pair(controller,
                  make_eval_stack_cell(controller,
                                       make_pair(controller, false_object, null_object),
                                       make_pair(controller, consumer, null_object),
                                       null_object,
                                       null_object),
                  controller->evaled);

    tmp_eval_stack =
        make_pair(controller,
                  make_eval_stack_cell(controller,
                                       make_pair(controller, syntax_pass_values_object, null_object),
                                       make_pair(controller, syntax_pass_values_object, null_object),
                                       null_object,
                                       null_object),
                  tmp_eval_stack);
    controller->evaled = null_object;
    return apply_aux(controller, producer, null_object, tmp_eval_stack);
}

ScmObject cact_apply(cactus_runtime_controller controller,int n_args, ScmObject *arg_array){

}

ScmObject cact_write_char(cactus_runtime_controller controller, int n_args, ScmObject *arg_array){
    assert(n_args == 1|| n_args == 2);
    ScmObject port;
    if (n_args == 1){
        port = make_output_file_port(controller, stdout);
    }else if (n_args == 2){
        port = arg_array[1];
    }
    assert( output_port_p(port));
    write_char(controller,arg_array[0], port);
    return null_object;
}
