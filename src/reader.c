#include "cactus.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

scm_object simple_read(FILE* file, cactus_runtime_controller controller);

static int space_p(char c){
    return  c == ' ' || c == '\n';
}

static int delimiter_p(char c){
    return c == '(' || c == ')'  || space_p(c);
}

static scm_object simple_read_list(FILE* file, cactus_runtime_controller controller){
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
        scm_object obj = simple_read(file,controller);
        set_cdr(res_cell,make_pair(null_object,null_object));
        res_cell = ref_cdr(res_cell);
        set_car(res_cell, obj);
    }
}

static scm_object simple_read_non_pair(FILE* file, cactus_runtime_controller controller){
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
        scm_symbol symbol =  make_symbol(buff);
        return symbol_intern(symbol, controller->symbol_intern);
    }
}

scm_object simple_read(FILE* file, cactus_runtime_controller controller){
    char first_c;
    while (space_p(first_c = getc(file)));
    ungetc(first_c, file);

    if (first_c == '('){
        return simple_read_list(file, controller);
    }else{
        return simple_read_non_pair(file, controller);
    }
}

