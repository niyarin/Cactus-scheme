#include<stdlib.h>
#include "cactus.h"

ScmObject make_closure(cactus_runtime_controller controller, ScmObject formals,ScmObject body ,ScmObject env){
    Closure closure = (Closure)malloc(sizeof(struct closure));
    if (!closure){
        exit(1);
    }
    closure->formals = formals;
    closure->body = body;
    closure->env = env;
    return make_scm_object(controller, TYPE_CLOSURE, (uintptr_t)closure);
}

ScmObject ref_closure_body(ScmObject closure){
    Closure closure_structure = ref_object_value(closure);
    return closure_structure->body;
}

ScmObject ref_closure_formals(ScmObject closure){
    Closure closure_structure = ref_object_value(closure);
    return closure_structure->formals;
}

ScmObject make_continuation(cactus_runtime_controller controller){
    return make_scm_object(controller, TYPE_CONTINUATION, (uintptr_t)controller->eval_stack);
}

scm_object make_primitive(cactus_runtime_controller controller, primitive_procedure fn){
    return make_const_scm_object(controller, TYPE_PRIMITIVE, (uintptr_t)fn);
}

int closure_p(scm_object object){
    return ref_object_type(object) == TYPE_CLOSURE;
}

int primitive_p(scm_object object){
    return ref_object_type(object) == TYPE_PRIMITIVE;
}

int continuation_p(ScmObject object){
    return ref_object_type(object) == TYPE_CONTINUATION;
}

ScmObject ref_continuation_eval_stack(ScmObject continuation){
    return (ScmObject)ref_object_value(continuation);
}
