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
    }else if (obj == true_object){
        fprintf(file, "#t");
    }else if (obj == false_object){
        fprintf(file, "#f");
    }else if (obj == false_object){
    }else{
        fprintf(file, "#<??>");
    }
}

void boot_pair(cactus_runtime_controller controller){
    //cons
    scm_symbol cons_symbol = make_const_symbol(controller, "cons");
    symbol_intern(controller, cons_symbol);
    add_global(controller, cons_symbol, make_primitive(controller, &cact_cons));

    symbol_intern(controller, make_const_symbol(controller, "car"));
    symbol_intern(controller, make_const_symbol(controller, "cdr"));
    symbol_intern(controller, make_const_symbol(controller, "set-car!"));
    symbol_intern(controller, make_const_symbol(controller, "set-cdr!"));
}

void boot_syntax(cactus_runtime_controller controller){
    scm_symbol quote_symbol = make_const_symbol(controller, "quote");
    symbol_intern(controller, quote_symbol);
    add_global_syntax(controller, quote_symbol, syntax_quote_object);
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
}

void boot(cactus_runtime_controller controller){
    boot_controller(controller);
    boot_pair(controller);
    boot_syntax(controller);
}

int main(void){
    printf("Hello cactus.\n");

    struct cactus_runtime_controller_t controller;
    boot(&controller);

    scm_object input = simple_read(stdin, &controller);
    simple_write(stdout, input);printf("\n");
    simple_write(stdout, true_object);printf("\n");
    simple_write(stdout, false_object);printf("\n");

    scm_object dead = make_pair(&controller, make_const_symbol(&controller,"ABC"),make_const_symbol(&controller,"EFG"));

    scm_object piyo = make_const_symbol(&controller,"EFG");
    scm_object ephemeron = make_ephemeron(&controller,piyo,piyo);

    gc_add_root(&controller, ephemeron);
    gc_add_root(&controller, input);
    gc_add_root(&controller, controller.symbol_intern);

    {
        scm_symbol _cons_symbol = make_const_symbol(&controller, "cons");
        scm_symbol cons_symbol = symbol_intern(&controller, _cons_symbol);
        simple_write(stdout, eval(&controller, input));
    }

    gc(&controller);
    printf("%ld\n", ephemeron_broken_p(ephemeron));
    return 0;
}
