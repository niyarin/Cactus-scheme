#include<stdlib.h>
#include "cactus.h"

ScmObject make_char(cactus_runtime_controller controller, uint32_t u32_char){
    return make_const_scm_object(controller,  TYPE_CHAR,  u32_char);
}
