#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "cactus.h"

void simple_write(FILE *file, scm_object obj){
    if (obj->type == TYPE_FIX_NUM){
        fprintf(file,"%d",(int)obj->value);
    }else if (obj->type == TYPE_PAIR){
        fprintf(file,"(");
        simple_write(file,ref_car(obj));
        fprintf(file," . ");
        simple_write(file,ref_cdr(obj));
        fprintf(file,")");
    }else if (obj->type == TYPE_SYMBOL){
        fprintf(file, "%s", (char*)obj->value);
    }else if (obj->type == TYPE_NULL){
        fprintf(file, "()");
    }else if (obj->type == TYPE_PRIMITIVE){
        fprintf(file, "#<primitive-procedure>");
    }else{
        fprintf(file, "#<??>");
    }
}

int plus_ope_p(scm_object obj){
    if (symbol_p(obj)){
        char* val = (char*)obj->value;
        return val[0] == '+' && val[1] == '\0';
    }
    return 0;
}

int mul_ope_p(scm_object obj){
    if (symbol_p(obj)){
        char* val = (char*)obj->value;
        return val[0] == '*' && val[1] == '\0';
    }
    return 0;
}

scm_object simple_calc(scm_object expression){
    if (pair_p(expression)){
        scm_object ope = ref_car(expression);
        scm_object args = ref_cdr(expression);
        if (mul_ope_p(ope)){
            int res_val = 1;
            while (!null_p(args)){
                res_val *= (simple_calc(ref_car(args)))->value;
                args = ref_cdr(args);
            }
            return make_fixnum(res_val);
        }else if (plus_ope_p(ope)){
            int res_val = 0;
            while (!null_p(args)){
                res_val += (simple_calc(ref_car(args)))->value;
                args = ref_cdr(args);
            }
            return make_fixnum(res_val);
        }else{
            return make_fixnum(0);
        }
    }else if (fixnum_p(expression)){
        return expression;
    }
    return make_fixnum(0);
}

void boot_pair(cactus_runtime_controller controller){
    scm_pair intern_box = controller->symbol_intern;
    symbol_intern(make_const_symbol("cons"), intern_box);
    symbol_intern(make_const_symbol("car"), intern_box);
    symbol_intern(make_const_symbol("cdr"), intern_box);
    symbol_intern(make_const_symbol("set-car!"), intern_box);
    symbol_intern(make_const_symbol("set-cdr!"), intern_box);
}

void boot(cactus_runtime_controller controller){
    scm_pair intern_box = make_pair(null_object, null_object);
    controller->symbol_intern = intern_box;
    controller-> all_objects_area_size = 32;
    controller-> all_objects_size = 0;
    controller->all_objects = (scm_object*)malloc(sizeof(scm_object) * controller-> all_objects_area_size);
    if (controller->all_objects == NULL){
        exit(1);
    }
    controller->gc_roots = null_object;

    boot_pair(controller);
}

int main(void){
    printf("Hello cactus.\n");

    struct cactus_runtime_controller_t controller;
    boot(&controller);

    scm_object input = simple_read(stdin, &controller);

    scm_object x = make_pair( make_const_symbol("OK1"), make_const_symbol("OK2"));
    scm_object y = make_primitive(&cact_cdr);
    scm_object z =  call_scm_primitive(&controller, y, 1 ,x);
    simple_write(stdout,input); printf("\n");
    simple_write(stdout,x); printf("\n");
    simple_write(stdout,y); printf("\n");
    simple_write(stdout,z); printf("\n");
    return 0;
}
