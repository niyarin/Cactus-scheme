#include <assert.h>
#include "cactus.h"

scm_object copy_list(cactus_runtime_controller controller, scm_object list){
    assert(pair_p(list));
    scm_pair cell = list;
    scm_pair res = make_pair(controller, null_object, null_object);
    scm_pair res_cell = res;
    while (!null_p(cell)){
        set_cdr(res_cell, make_pair(controller, ref_car(cell), null_object));
        res_cell = ref_cdr(res_cell);
        cell = ref_cdr(cell);
    }
    return ref_cdr(res);
}

scm_object list_reverse(cactus_runtime_controller controller, scm_object list){
    assert(pair_p(list));
    scm_pair cell = list;
    scm_pair res = null_object;
    while (!null_p(cell)){
        res = make_pair(controller, ref_car(cell), res);
        cell = ref_cdr(cell);
    }
    return res;
}

int list_length(scm_object list){
    assert(pair_p(list));
    scm_pair cell = list;
    int res = 0;
    while (!null_p(cell)){
        res++;
        cell = ref_cdr(cell);
    }
    return res;

}
