#include <assert.h>
#include "cactus.h"

ScmObject make_vector(cactus_runtime_controller controller, size_t length){
    ScmObject* vector_container = (ScmObject*)malloc(sizeof(ScmObject) * length);
    if (!vector_container){
        exit(1);
    }
    VectorInfo vector_info = (VectorInfo)malloc(sizeof(struct vector_info));
    if (!vector_info){
        exit(1);
    }

    for (int i=0;i<length;i++){
        vector_container[i] = null_object;
    }
    vector_info->vector_container = vector_container;
    vector_info->length = length;

    return make_scm_object(controller, TYPE_VECTOR, (uintptr_t)vector_info);
}

ScmObject vector_ref(ScmObject vector, int index){
    VectorInfo vector_info =  (VectorInfo)ref_object_value(vector);
    assert(index < vector_info->length);
    ScmObject* vector_container = vector_info->vector_container;
    return vector_container[index];
}

void vector_set(ScmObject vector, int index, ScmObject tgt){
    VectorInfo vector_info =  (VectorInfo)ref_object_value(vector);
    assert(index < vector_info->length);
    ScmObject* vector_container = vector_info->vector_container;
    vector_container[index] = tgt;
}

int vector_length(ScmObject vector){
    VectorInfo vector_info =  (VectorInfo)ref_object_value(vector);
    return vector_info->length;
}
