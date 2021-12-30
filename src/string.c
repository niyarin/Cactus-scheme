#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<assert.h>
#include "cactus.h"

ScmObject make_char(cactus_runtime_controller controller, uint32_t u32_char){
    return make_const_scm_object(controller,  TYPE_CHAR,  u32_char);
}

ScmObject make_string(cactus_runtime_controller controller,uint32_t* u32_array){
    return make_scm_object(controller, TYPE_STRING, (uintptr_t)u32_array);
}

ScmObject make_string_from_cstr(cactus_runtime_controller controller,char* s){
    int len = strlen(s);
    uint32_t *buff = (uint32_t*)malloc(sizeof(uint32_t) * (len+1));
    for (int i=0;i<len;i++){
        buff[i] = s[i];
    }
    buff[len] = 0;
    return make_scm_object(controller, TYPE_STRING, (uintptr_t)buff);
}

void strcopy_from_scm_str(char* buff, ScmObject scm_str){
    //not support u32
    int i=0;
    uint32_t *from = (uint32_t*)ref_object_value(scm_str);
    while (from[i]){
        assert(from[i] < 128);
        buff[i] = from[i];
        i++;
    }
    buff[i] = 0;
}

ScmObject make_string_const(cactus_runtime_controller controller,uint32_t* u32_array){
    return make_const_scm_object(controller, TYPE_STRING, (uintptr_t)u32_array);
}

int string_p(scm_object object){
    return ref_object_type(object) == TYPE_STRING;
}

ScmObject string_append(cactus_runtime_controller controller, int n_args, ...){
    int buff_size = 128;
    int used_buff = 0;
    uint32_t *buff = (uint32_t*)malloc(sizeof(uint32_t) * buff_size);

    va_list ap;
    va_start(ap, n_args);
    for (int i=0;i<n_args;i++){
        ScmObject str = va_arg(ap, ScmObject);
        uint32_t *from = (uint32_t*)ref_object_value(str);
        int j=0;
        while (from[j]){
            if (buff_size+1 == used_buff){
                buff_size *= 2;
                buff = (uint32_t*)realloc(buff, buff_size);
            }
            buff[used_buff] = from[j];
            j++;
            used_buff++;
        }
    }
    buff[used_buff] = 0;
    va_end(ap);

    return make_string(controller, buff);
}
