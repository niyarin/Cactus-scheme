#include <stdlib.h>
#include<assert.h>
#include "cactus.h"

void add_all_objects(cactus_runtime_controller controller, scm_object object){
    if (controller-> all_objects_area_size == controller-> all_objects_size){
        controller->all_objects_area_size *= 2;
        controller->all_objects = realloc(controller->all_objects,
                                          sizeof(scm_object) *  controller->all_objects_area_size);
        if (controller->all_objects == NULL){
            exit(1);
        }
    }
    controller->all_objects[controller->all_objects_size] = object;
    controller->all_objects_size++;
}

void gc_add_root(cactus_runtime_controller controller, scm_object object){
    controller->gc_roots = make_pair(controller, object, controller->gc_roots);
}

void gc_reset_mark(cactus_runtime_controller controller){
    int i;
    for (i=0;i<controller-> all_objects_size;i++){
        scm_object obj = controller->all_objects[i];
        if (!null_p(obj)){
            obj->mark = 0;
        }
    }
}

static void gc_mark(scm_object obj){
    obj->mark = 1;
}

static void gc_mark_loop(cactus_runtime_controller controller, scm_object obj){
    gc_mark(obj);
    if (ref_object_type(obj) == TYPE_PAIR){
        gc_mark_loop(controller, ref_car(obj));
        gc_mark_loop(controller, ref_cdr(obj));
    }else if (ref_object_type(obj) == TYPE_EPHEMERON){
        if(controller->ephemeron_queue_area_size == controller->ephemeron_queue_size){
            controller->ephemeron_queue_area_size *= 2;
            controller->ephemeron_queue =
                (scm_object*)realloc(controller->ephemeron_queue,
                        controller->ephemeron_queue_area_size * sizeof(scm_object));
        }
        controller->ephemeron_queue[controller->ephemeron_queue_size] = obj;
        controller->ephemeron_queue_size++;
    }else{
    }
}

void gc_mark_phase(cactus_runtime_controller controller){

    controller->ephemeron_queue_area_size = 8;
    controller->ephemeron_queue =
        (scm_object*)malloc(controller->ephemeron_queue_area_size * sizeof(scm_object));
    controller->ephemeron_queue_size = 0;

    scm_object root_cell = controller->gc_roots;
    while (!null_p(root_cell)){
        assert(pair_p(root_cell));
        gc_mark_loop(controller, ref_car(root_cell));
        root_cell = ref_cdr(root_cell);
    }
}
