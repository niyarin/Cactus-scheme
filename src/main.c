#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
    }else if (obj->type == TYPE_SYMBOL){
        fprintf(file, "%s", (char*)obj->value);
    }else if (obj->type == TYPE_NULL){
        fprintf(file, "()");
    }
}

static int space_p(char c){
    return  c == ' ' || c == '\n';
}

static int delimiter_p(char c){
    return c == '(' || c == ')'  || space_p(c);
}

scm_object simple_read(FILE* file);

static scm_object simple_read_list(FILE* file){
    getc(file);//read '('
    scm_object res_cell_top = make_pair(null_object, null_object),
               res_cell = res_cell_top;

    while (1){
        char first_c;
        while (space_p(first_c = getc(file)));
        if (first_c == ')'){
            return ref_cdr(res_cell_top);
        }
        ungetc(first_c, file);
        scm_object obj = simple_read(file);
        //res_cell = make_pair(obj, res_cell);
        set_cdr(res_cell,make_pair(null_object,null_object));
        res_cell = ref_cdr(res_cell);
        set_car(res_cell, obj);
    }
}

static scm_object simple_read_non_pair(FILE* file){
    size_t buff_size = 32;
    char* buff = (char*)malloc(buff_size + 1);

    size_t current_position = 0;
    int number_literal_flag = 1;

    while (1){
        char c = getc(file);
        if (delimiter_p(c)){
            ungetc(c,file);
            break;
        }
        if (current_position == buff_size){
            buff_size *= 2;
            buff = (char*)realloc(buff, buff_size + 1);
        }

        if  (c < '0' || '9' < c){
            number_literal_flag = 0;
        }

        buff[current_position] = c;
        current_position++;
    }

    buff[current_position] = '\0';

    if (number_literal_flag){
        int n = atoi(buff);
        free(buff);
        return make_fixnum(n);
    }else{
        return make_symbol(buff);
    }
}

scm_object simple_read(FILE* file){
    char first_c;
    while (space_p(first_c = getc(file)));
    ungetc(first_c, file);

    if (first_c == '('){
        return simple_read_list(file);
    }else{
        return simple_read_non_pair(file);
    }
}

int main(void){
    printf("Hello cactus.\n");
    simple_write(stdout,simple_read(stdin));
    printf("\n");
    return 0;
}
