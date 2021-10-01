#include<stdio.h>
#include "cactus.h"

void simple_write(FILE *file, scm_object obj){
    if (obj->type == TYPE_FIX_NUM){
        fprintf(file,"%d",(int)obj->value);
    }else if (obj->type == TYPE_PAIR){
        fprintf(file,"(");
        simple_write(file,ref_car(obj));
        fprintf(file," . ");
        simple_write(file,ref_cdr(obj));
        fprintf(file,")");
    }
}

int main(void){
    printf("Hello cactus.\n");
    simple_write(stdout, make_fixnum(3));
    printf("\n");

    simple_write(stdout, make_pair(make_pair(make_fixnum(0),make_fixnum(1)),
                                   make_fixnum(2)));
    printf("\n");
    return 0;
}
