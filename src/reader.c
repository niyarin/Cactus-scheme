#include "cactus.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<assert.h>

scm_object simple_read(FILE* file, cactus_runtime_controller controller);

static int space_p(char c){
    return  c == ' ' || c == '\n';
}

static int delimiter_p(char c){
    return c == '(' || c == ')'  || space_p(c);
}

static scm_object simple_read_list(FILE* file, cactus_runtime_controller controller){
    getc(file);//read '('
    scm_object res_cell_top = make_pair(controller, null_object, null_object),
               res_cell = res_cell_top;

    while (1){
        char first_c;
        while (space_p(first_c = getc(file)));
        if (first_c == ')'){
            return ref_cdr(res_cell_top);
        }
        ungetc(first_c, file);
        scm_object obj = simple_read(file,controller);
        set_cdr(res_cell,make_pair(controller, null_object,null_object));
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
        return make_fixnum(controller, n);
    }else if (buff[0] == '"'){
        uint32_t *new_buff = (uint32_t*)malloc(sizeof(uint32_t) * (current_position-1));
        if (!new_buff){
            assert(0);
        }
        int i;
        for (i=1;i<current_position-1;i++){
            new_buff[i-1] = buff[i];
        }
        new_buff[current_position-2] = 0;
        free(buff);
        return make_string(controller, new_buff);
    }else{
        scm_symbol symbol =  make_symbol(controller, buff);
        return symbol_intern(controller, symbol);
    }
}

scm_object simple_read(FILE* file, cactus_runtime_controller controller){
    char first_c;
    while (space_p(first_c = getc(file)));
    ungetc(first_c, file);

    if (first_c == '('){
        return simple_read_list(file, controller);
    }else if (first_c == '#'){
        first_c = getc(file);
        char second_c = getc(file);
        if (second_c == '\\'){
            char third_c = getc(file);
            return make_char(controller, third_c);
        }
    }else{
        return simple_read_non_pair(file, controller);
    }
}

