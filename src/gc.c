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

static void gc_reset_mark(cactus_runtime_controller controller){
    int i;
    for (i=0;i<controller-> all_objects_size;i++){
        scm_object obj = controller->all_objects[i];
        if (!null_p(obj)){
            obj->mark = 0;
        }
    }
}

static void mark_object(scm_object obj){
    obj->mark = 1;
}

static void gc_mark_loop(cactus_runtime_controller controller, scm_object obj){
    if (obj->mark){
        return;
    }
    mark_object(obj);
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

static void gc_mark(cactus_runtime_controller controller){

    controller->ephemeron_queue_area_size = 8;
    controller->ephemeron_queue =
        (scm_object*)malloc(controller->ephemeron_queue_area_size * sizeof(scm_object));
    controller->ephemeron_queue_size = 0;
    gc_mark_loop(controller, controller->global);
    gc_mark_loop(controller, controller->macro_env);
    gc_mark_loop(controller, controller->local_stack);
    gc_mark_loop(controller, controller->symbol_intern);
    gc_mark_loop(controller, controller->load_path);
    gc_mark_loop(controller, controller->gc_roots);
}

static void ephemeron_mark(cactus_runtime_controller controller){
    int i;
    char contain_reachable_object = 0;
    size_t old_ephemeron_eueue_size = controller->ephemeron_queue_size;
    for (i=0;i<old_ephemeron_eueue_size;i++){
        scm_object key_object = ephemeron_key(controller->ephemeron_queue[i]);
        if (!null_p(key_object) && key_object->mark == 1){
            //value以外からkeyへの参照がある
            gc_mark_loop(controller, ephemeron_datum(controller->ephemeron_queue[i]));
            controller->ephemeron_queue[i] = null_object;
            contain_reachable_object = 1;
        }
    }

    if (contain_reachable_object){
        for (i=0;i<controller->ephemeron_queue_size;i++){
            if (null_p(controller->ephemeron_queue[i])){
                controller->ephemeron_queue[i] = controller->ephemeron_queue[controller->ephemeron_queue_size-1];
                controller->ephemeron_queue[controller->ephemeron_queue_size-1] = null_object;
                controller->ephemeron_queue_size--;
            }
        }
        ephemeron_mark(controller);
    }
}

static void apply_ephemeron_break(cactus_runtime_controller controller){
    int i;
    for (i=0;i<controller->ephemeron_queue_size;i++){
        ephemeron_break(controller->ephemeron_queue[i]);
    }
}

static void gc_free(scm_object obj){
    char type = ref_object_type(obj);
    if (type == TYPE_PAIR){
        free((void*)(ref_object_value(obj)));
        free((void*)obj);
    }else if (type == TYPE_EPHEMERON){
        free((void*)(ref_object_value(obj)));
        free(obj);
    }else if (type == TYPE_SYMBOL || type == TYPE_STRING){
        if (!obj-> value_is_not_reference){
            free((void*)(ref_object_value(obj)));
        }
        free((void*)obj);
    }else if (type == TYPE_NULL){
        //
    }else{
        free((void*)obj);
    }
}

void sweep(cactus_runtime_controller controller){
    int i;
    for (i=0;i<controller-> all_objects_size;i++){
        scm_object obj = controller->all_objects[i];
        if (null_p(obj)){
        }else if (!(obj->mark)){
            gc_free(obj);
            controller->all_objects[i] = null_object;
        }
    }
}

void gc(cactus_runtime_controller controller){
    gc_reset_mark(controller);
    gc_mark(controller);
    ephemeron_mark(controller);
    apply_ephemeron_break(controller);
    sweep(controller);
}
