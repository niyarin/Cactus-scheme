#include <assert.h>
#include <stdio.h>
#include "cactus.h"


scm_object pass_value(cactus_runtime_controller controller, scm_object value){
    if (null_p(controller->eval_stack)){
        controller->evaled = value;
        return null_object;
    }
    scm_pair stack_top = ref_car(controller->eval_stack);
    controller->evaled = make_pair(controller, value, vector_ref(stack_top, 1));
    controller->un_evaled = vector_ref(stack_top,2);
    controller->local_stack = vector_ref(stack_top,3);
    controller->eval_stack = ref_cdr(controller->eval_stack);
    return vector_ref(stack_top,0);
}

scm_object pass_values(cactus_runtime_controller controller, scm_object values){
    controller->eval_stack = ref_cdr(controller->eval_stack);

    if (null_p(controller->eval_stack)){
        controller->evaled = ref_car(values);
        return null_object;
    }
    scm_pair stack_top = ref_car(controller->eval_stack);
    controller->evaled = vector_ref(stack_top, 1);
    {
        ScmObject cell = values;
        while (!null_p(cell)){
            controller->evaled = make_pair(controller, ref_car(cell), controller->evaled);
            cell = ref_cdr(cell);
        }
    }

    controller->un_evaled = vector_ref(stack_top,2);
    controller->local_stack = vector_ref(stack_top,3);
    controller->eval_stack = ref_cdr(controller->eval_stack);
    return vector_ref(stack_top,0);
}


ScmObject make_eval_stack_cell(cactus_runtime_controller controller, ScmObject expression, ScmObject evaled, ScmObject un_evaled, ScmObject local_stack){
    ScmObject res = make_vector(controller,4);
    vector_set(res, 0 , expression);
    vector_set(res, 1 , evaled);
    vector_set(res, 2 , un_evaled);
    vector_set(res, 3 , local_stack);
    return res;
}

ScmObject apply(cactus_runtime_controller controller, ScmObject procedure, ScmObject args){
    if ( primitive_p(procedure)){
        int n_args = list_length(args);
        scm_object* args_array = (scm_object*)malloc(sizeof(scm_object) * n_args);
        if (!args_array){
            exit(1);
        }
        int arg_pos = 0;
        scm_pair cell = args;
        arg_pos = 0;
        while (!null_p(cell)){
            args_array[arg_pos] = ref_car(cell);
            arg_pos++;
            cell = ref_cdr(cell);
        }
        primitive_procedure primitive = (primitive_procedure)ref_object_value(procedure);
        scm_object apply_res =  primitive(controller, n_args, args_array);

        return pass_value(controller, apply_res);
    }else if (closure_p(procedure)){
         ScmObject stack_cell = null_object;
         ScmObject _args = args;
         ScmObject _formals = ref_closure_formals(procedure);
         while (1){
            if (null_p(_formals)){
                break;
            }else if (!pair_p(_formals)){
                stack_cell = make_pair(controller, make_pair(controller, _formals, _args),stack_cell);
                break;
            }
            assert(pair_p(_args));
            stack_cell = make_pair(controller, make_pair(controller, ref_car(_formals), ref_car(_args)),stack_cell);
            _args = ref_cdr(_args);
            _formals = ref_cdr(_formals);
         }
         controller->local_stack = make_pair(controller, stack_cell, null_object);
         controller->evaled = null_object;
         return ref_closure_body(procedure);
    }else if (continuation_p(procedure)){
        controller->eval_stack = ref_continuation_eval_stack(procedure);
        if (!null_p(controller->eval_stack)){
            ScmObject next_operator = ref_car(vector_ref(ref_car(controller->eval_stack),1));
            if (next_operator == syntax_pass_values_object){
                return pass_values(controller,args);
            }
        }
        return pass_value(controller, ref_car(args));//TODO:values
    }else{
        assert(0);
    }
}

static scm_object apply_procedure_step(cactus_runtime_controller controller, scm_object expression){
    if (controller->evaled == null_object){
        controller->un_evaled = expression;
    }else if (controller->un_evaled == null_object){
        scm_object evaled_expression = list_reverse(controller, controller->evaled);
        scm_object operator = ref_car(evaled_expression);
        return apply(controller, operator, ref_cdr(evaled_expression));
    }

    controller->eval_stack =
        make_pair(controller,
                  make_eval_stack_cell(controller, expression, controller->evaled, ref_cdr(controller->un_evaled), controller->local_stack),
                  controller->eval_stack);
    controller->evaled = null_object;
    return ref_car(controller->un_evaled);
}

scm_object eval_step(cactus_runtime_controller controller, scm_object expression){
    if (pair_p(expression)){
        if (ref_car(expression) == syntax_quote_object){
            return pass_value(controller, ref_car(ref_cdr(expression)));
        }else if (ref_car(expression) == syntax_lambda_object){
            ScmObject closure =
                make_closure(controller,
                        ref_car(ref_cdr(expression)),
                        ref_car(ref_cdr(ref_cdr(expression))),
                        null_object);
            return pass_value(controller, closure);
        }else if (ref_car(expression) == syntax_define_object){
            if (null_p(controller->evaled)){
                controller->eval_stack =
                    make_pair(controller,
                            make_eval_stack_cell(controller, expression, null_object, null_object, controller->local_stack),
                            controller->eval_stack);
                return ref_car(ref_cdr(ref_cdr(expression)));
            }

            if (controller->local_stack != null_object){
                assert(0);
            }else{
                add_global(controller, ref_car(ref_cdr(expression)), ref_car(controller->evaled));
                return pass_value(controller, null_object);
            }
        }else if (ref_car(expression) == syntax_set_object){
            if (null_p(controller->evaled)){
                controller->eval_stack =
                    make_pair(controller,
                            make_eval_stack_cell(controller, expression, null_object, null_object, controller->local_stack),
                            controller->eval_stack);
                return ref_car(ref_cdr(ref_cdr(expression)));
            }
            update(controller, ref_car(ref_cdr(expression)), ref_car(controller->evaled));
            return pass_value(controller, null_object);
        }else if (ref_car(expression) == syntax_call_with_values_internal_object){
            if (null_p(controller->evaled)){
                controller->eval_stack =
                    make_pair(controller,
                            make_eval_stack_cell(controller, expression, null_object,
                                ref_cdr(expression),
                                controller->local_stack),
                            controller->eval_stack);
                return ref_car(ref_cdr(ref_cdr(expression)));
            }else if (! null_p(ref_cdr(controller->evaled))){
                ScmObject producer = ref_car(controller->evaled),
                          consumer = ref_car(ref_cdr(controller->evaled));

                controller->evaled = null_object;
                ScmObject apply_procedure =
                  make_pair(controller,
                        make_pair(controller,
                                  syntax_quote_object,
                                  make_pair(controller,
                                            producer,
                                            null_object)),
                        null_object);

                ScmObject pass_values_code =
                    make_pair(controller,
                              syntax_pass_values_object,
                              make_pair(controller,
                              apply_procedure,
                              null_object));

                ScmObject submit_consumer =
                    make_pair(controller,
                              make_pair(controller,
                                  syntax_quote_object,
                                  make_pair(controller,
                                            consumer,
                                            null_object)),
                              make_pair(controller,pass_values_code,null_object));
                return submit_consumer;
            }
            controller->eval_stack =
                make_pair(controller,
                          make_eval_stack_cell(controller, expression, controller->evaled, ref_cdr(controller->un_evaled), controller->local_stack),
                          controller->eval_stack);
            controller->evaled = null_object;
            return ref_car(controller->un_evaled);
        }else if (ref_car(expression) == syntax_pass_values_object){
            controller->eval_stack =
                make_pair(controller,
                          make_eval_stack_cell(controller, expression,
                              make_pair(controller,syntax_pass_values_object,null_object), null_object, controller->local_stack),
                          controller->eval_stack);
            controller->evaled = null_object;
            return ref_car(ref_cdr(expression));
        }else if (ref_car(expression) == syntax_call_cc_internal_object){
            if (null_p(controller->evaled)){
                controller->eval_stack =
                    make_pair(controller,
                            make_eval_stack_cell(controller, expression, null_object, null_object, controller->local_stack),
                            controller->eval_stack);
                return ref_car(ref_cdr(expression));
            }
            ScmObject continuation = make_continuation(controller);
            controller->evaled = make_list2(controller, continuation, ref_car(controller->evaled));
            return make_pair(controller, null_object, null_object);
        }else{
            return apply_procedure_step(controller, expression);
        }
    }else if (symbol_p(expression)){
        return pass_value(controller, lookup(controller, expression));
    }else{
        return pass_value(controller, expression);
    }
    assert(0);
}

/*
scm_object solve_syntax(cactus_runtime_controller controller, scm_object expression){
    if (pair_p(expression)){
        scm_object operator = lookup_syntax(controller, ref_car(expression));
        if (operator && built_in_syntax_p(operator)){
            switch (ref_object_value(operator)){
                case SYNTAX_QUOTE_ID:
                    return make_pair(controller,
                                     operator,
                                     ref_cdr(expression));
                case SYNTAX_LAMBDA_ID:
                    {
                        scm_pair res = copy_list(controller, expression);
                        set_car(ref_cdr(ref_cdr(res)),
                                solve_syntax(controller, ref_car(ref_cdr(ref_cdr(expression)))));
                        set_car(res,  operator);
                        return res;
                    }
                case SYNTAX_DEFINE_ID:
                    {
                        if (pair_p(ref_car(ref_cdr(expression)))){
                            assert(0);
                        }
                        //no break
                    }
                case SYNTAX_SET_ID:
                    {
                        scm_pair res = copy_list(controller, expression);
                        set_car(ref_cdr(ref_cdr(res)),
                                solve_syntax(controller, ref_car(ref_cdr(ref_cdr(expression)))));
                        set_car(res,  operator);
                        return res;
                    }
                case SYNTAX_IF_ID:
                    {
                        if (null_p(ref_cdr(ref_cdr(ref_cdr(expression))))){
                            assert(0);
                        }
                        scm_pair res = copy_list(controller, expression);
                        set_car(ref_cdr(expression),
                                solve_syntax(controller, ref_car(ref_cdr(expression))));
                        set_car(ref_cdr(ref_cdr(expression)),
                                solve_syntax(controller, ref_car(ref_cdr(ref_cdr(expression)))));
                        set_car(ref_cdr(ref_cdr(ref_cdr(expression))),
                                solve_syntax(controller, ref_car(ref_cdr(ref_cdr(ref_cdr(expression))))));
                        set_car(res,  operator);
                        return res;
                    }
                case SYNTAX_CALL_CC_INTERNAL_ID:
                    {
                        scm_pair res = copy_list(controller, expression);
                        set_car(ref_cdr(res),
                                solve_syntax(controller, ref_car(ref_cdr(expression))));
                        set_car(res,  operator);
                        return res;
                    }
                case SYNTAX_CALL_WITH_VALUES_INTERNAL_ID:
                    {
                        scm_pair res = copy_list(controller, expression);
                        set_car(ref_cdr(res),
                                solve_syntax(controller, ref_car(ref_cdr(expression))));
                        set_car(ref_cdr(ref_cdr(res)),
                                solve_syntax(controller, ref_car(ref_cdr(ref_cdr(expression)))));
                        set_car(res,  operator);
                        return res;
                    }
            }
        }else{
            scm_object res = make_pair(controller, null_object, null_object);
            scm_object cell = expression;
            scm_object res_cell = res;
            while (!null_p(cell)){
                set_cdr(res_cell,
                        make_pair(controller, solve_syntax(controller, ref_car(cell)), null_object));
                res_cell = ref_cdr(res_cell);
                cell = ref_cdr(cell);
            }
            return ref_cdr(res);
        }
    }
    return expression;
}
*/

ScmObject formals2symbol_list(cactus_runtime_controller controller, ScmObject formals){
    if (symbol_p(formals)){
        return make_pair(controller, formals, null_object);
    }else if (null_p(formals)){
        return null_object;
    }else if (pair_p(formals)){
        return
            make_pair(controller,
                      ref_car(formals),
                      formals2symbol_list(controller, ref_cdr(formals)));

    }else{
        assert(0);
    }
}


ScmObject solve_syntax_internal(cactus_runtime_controller controller, ScmObject identifier_expression,
                               ScmObject runtime_env, ScmObject syntax_env, ScmObject counter){
    if (pair_p(identifier_expression)){
        ScmObject operator = lookup_syntax(runtime_env, ref_car(identifier_expression));
        if (operator && built_in_syntax_p(operator)){
            switch (ref_object_value(operator)){
                case SYNTAX_LAMBDA_ID:
                break;
            }
        }else{
            ScmObject res = make_pair(controller, null_object, null_object);
            ScmObject  cell = identifier_expression;
            scm_object res_cell = res;
            while (!null_p(cell)){
                set_cdr(res_cell,
                        make_pair(controller, solve_syntax_internal(controller, ref_car(cell), runtime_env, syntax_env, counter), null_object));
                res_cell = ref_cdr(res_cell);
                cell = ref_cdr(cell);
            }
            return ref_cdr(res);
        }
    }else{
        return identifier_expression;
    }
    assert(0);
}

ScmObject solve_syntax(cactus_runtime_controller controller, ScmObject identifier_expression){

    ScmObject runtime_env = controller-> macro_env;
    ScmObject res = solve_syntax_internal(controller, identifier_expression, runtime_env, runtime_env, make_fixnum(controller, 0));
    return res;
}



ScmObject call_eval_step(cactus_runtime_controller controller, scm_object solved_syntax_expression){
    scm_object next = solved_syntax_expression;
    for (int i=0;i<2000;i++){
         next = eval_step(controller, next);
         if (null_p(next)){
            break;
         }
    }
    return controller->evaled;
}

char import_string[] = "import";
struct scm_object_t import_symbol_entity = {TYPE_SYMBOL, 1, 0, (uintptr_t)&import_string};
ScmObject import_symbol = (scm_object)&import_symbol_entity;

char define_library[] = "define_library";
struct scm_object_t define_library_symbol_entity = {TYPE_SYMBOL, 1, 0, (uintptr_t)&import_string};
ScmObject define_library_symbol = (scm_object)&define_library_symbol_entity;

static int import_expression_p(ScmObject expression){
    return pair_p(expression)
           && symbol_p(ref_car(expression))
           && simple_symbol_eq(ref_car(expression), import_symbol);
}

static int define_library_expression_p(ScmObject expression){
    return pair_p(expression)
           && symbol_p(ref_car(expression))
           && simple_symbol_eq(ref_car(expression), define_library_symbol);

}

/*
static void eval_import(cactus_runtime_controller controller,
                        ScmObject import_expression){
}

static void eval_library_declaration(cactus_runtime_controller controller,
                                          ScmObject library_declaration){
    if (import_expression_p(library_declaration)){

    }
    assert(0);
}

static ScmObject eval_define_lirary(cactus_runtime_controller controller,
                                    ScmObject define_lirary_expression){
    ScmObject current_global = controller->global;
    ScmObject current_macro_env = controller->macro_env;
    gc_add_root(controller, current_global);
    gc_add_root(controller, current_macro_env);
    ScmList library_declarations = ref_cddr(define_lirary_expression);
    while (library_declarations != null_object){
        eval_library_declaration(controller, ref_car(library_declarations));
        library_declarations = ref_cdr(library_declarations);
    }

    controller->global = current_global;
    controller->macro_env = current_macro_env;
    gc_remove_root(controller, current_global);
    gc_remove_root(controller, current_macro_env);
}
*/

ScmObject convert_symbols_to_identifiers(cactus_runtime_controller controller, ScmObject expression){
    if (pair_p(expression)){
        return make_pair(controller,
                         convert_symbols_to_identifiers(controller, ref_car(expression)),
                         convert_symbols_to_identifiers(controller, ref_cdr(expression)));
    }else if (symbol_p(expression)){
        return make_identifier(controller, expression, false_object, false_object);
    }else{
        return expression;
    }
}

scm_object eval(cactus_runtime_controller controller, scm_object expression){
    controller->eval_stack = null_object;
    controller->evaled = null_object;
    controller->un_evaled = null_object;
    controller->local_stack = null_object;

    if (import_expression_p(expression)){
        return null_object;
    }else if (define_library_expression_p(expression)){
        return null_object;
    }else{
        ScmObject identifier_expression = convert_symbols_to_identifiers(controller, expression);
        ScmObject expanded_expression = solve_syntax(controller, identifier_expression);
        return expanded_expression;
        /*
        ScmObject solved_syntax_expression = solve_syntax(controller, expression);
        simple_write(stdout, solved_syntax_expression);printf("\n");
        return call_eval_step(controller, solved_syntax_expression);
        */
    }
}

static int is_exist_path_p(ScmObject path){
    char buff[1024];
    FILE *file;
    strcopy_from_scm_str(buff, path);
    if ((file = fopen(buff, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

ScmObject find_load_file(cactus_runtime_controller controller, ScmObject name){
    ScmObject path_list = controller->load_path;
    ScmObject slash_str = make_string_from_cstr(controller, "/");
    while (path_list != null_object){
        ScmObject path1 = string_append(controller, 3, ref_car(path_list), slash_str, name);
        if (is_exist_path_p(path1)){
            return path1;
        }
        ScmObject path2 = string_append(controller, 4,
                                        ref_car(path_list),
                                        slash_str,
                                        name,
                                        make_string_from_cstr(controller, ".scm"));
        if (is_exist_path_p(path2)){
            return path2;
        }
        ScmObject path3 = string_append(controller, 4,
                                        ref_car(path_list),
                                        slash_str,
                                        name,
                                        make_string_from_cstr(controller, ".sld"));
        if (is_exist_path_p(path3)){
            return path3;
        }
        path_list = ref_cdr(path_list);
    }
}


ScmObject load(cactus_runtime_controller controller, ScmObject name){
    ScmObject path = find_load_file(controller, name);
    char cpath[1024];
    strcopy_from_scm_str(cpath, path);
}
