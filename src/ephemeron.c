#include "cactus.h"

scm_object make_ephemeron(cactus_runtime_controller controller, scm_object key, scm_object datum){
    ephemeron_cell cell = (ephemeron_cell)malloc(sizeof(struct ephemeron_cell_t));
    if (!cell){
        exit(1);
    }
    cell->key = key;
    cell->datum = datum;
    cell->broken = 0;
    return make_scm_object(controller, TYPE_EPHEMERON, (uintptr_t)cell);
}

scm_object ephemeron_key(scm_ephemeron ephemeron){
    ephemeron_cell cell = (ephemeron_cell)ref_object_value(ephemeron);
    if (cell->broken == 0){
        return cell->key;
    }
    return null_object;
}

scm_object ephemeron_datum(scm_ephemeron ephemeron){
    ephemeron_cell cell = (ephemeron_cell)ref_object_value(ephemeron);
    if (cell->broken == 0){
        return cell->datum;
    }
    return null_object;
}

int ephemeron_p(scm_object object){
    return ref_object_type(object) == TYPE_EPHEMERON;
}
