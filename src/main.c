#include<stdio.h>
#include "cactus.h"

void simple_write(FILE *file, scm_object obj){
    if (obj->type == TYPE_FIX_NUM){
        fprintf(file,"%d",(int)obj->value);
    }
}

int main(void){
    printf("Hello cactus.\n");
    simple_write(stdout, make_fixnum(3));
    printf("\n");
    return 0;
}
