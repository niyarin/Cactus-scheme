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

void boot_pair(cactus_runtime_controller controller){
    printf("%ld\n", controller-> all_objects_size);
    symbol_intern(controller, make_const_symbol(controller, "cons"));
    symbol_intern(controller, make_const_symbol(controller, "car"));
    symbol_intern(controller, make_const_symbol(controller, "cdr"));
    symbol_intern(controller, make_const_symbol(controller, "set-car!"));
    symbol_intern(controller, make_const_symbol(controller, "set-cdr!"));
}

void boot(cactus_runtime_controller controller){

    controller-> all_objects_area_size = 32;
    controller-> all_objects_size = 0;
    controller->all_objects = (scm_object*)malloc(sizeof(scm_object) * controller-> all_objects_area_size);
    if (controller->all_objects == NULL){
        exit(1);
    }
    scm_pair intern_box = make_pair(controller, null_object, null_object);
    controller->symbol_intern = intern_box;
    controller->gc_roots = null_object;

    boot_pair(controller);
}

int main(void){
    printf("Hello cactus.\n");

    struct cactus_runtime_controller_t controller;
    boot(&controller);

    scm_object input = simple_read(stdin, &controller);
    scm_object dead = make_pair(&controller, make_const_symbol(&controller,"ABC"),make_const_symbol(&controller,"EFG"));

    scm_object piyo = make_const_symbol(&controller,"EFG");
    scm_object ephemeron = make_ephemeron(&controller,piyo,piyo);

    gc_add_root(&controller, ephemeron);
    gc_add_root(&controller, input);
    gc_add_root(&controller, controller.symbol_intern);
    gc(&controller);

    printf("%ld\n",ephemeron_broken_p(ephemeron));

    return 0;
}
