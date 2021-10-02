#include<string.h>
#include<assert.h>
#include "cactus.h"

scm_object lookup(cactus_runtime_controller controller, scm_symbol var){
    scm_object apair = assq(var, controller->global);
    if (apair !=  false_object){
        return ref_cdr(ref_car(apair));
    }else{
        //lookup local
        assert(0);
    }
}

void add_global(cactus_runtime_controller controller,
                scm_symbol var,
                scm_object val){
    controller->global = make_pair(controller, make_pair(controller,var,val),
                                               controller->global);
}
