#include <stdlib.h>
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
