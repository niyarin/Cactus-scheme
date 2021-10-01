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
    }else if (obj->type == TYPE_PRIMITIVE){
        fprintf(file, "#<primitive-procedure>");
    }else{
        fprintf(file, "#<??>");
    }
}

static int space_p(char c){
    return  c == ' ' || c == '\n';
}

static int delimiter_p(char c){
    return c == '(' || c == ')'  || space_p(c);
}

scm_object simple_read(FILE* file, scm_pair intern_box);

static scm_object simple_read_list(FILE* file, scm_pair intern_box){
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
        scm_object obj = simple_read(file,intern_box);
        set_cdr(res_cell,make_pair(null_object,null_object));
        res_cell = ref_cdr(res_cell);
        set_car(res_cell, obj);
    }
}

static scm_object simple_read_non_pair(FILE* file, scm_pair intern_box){
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
        return symbol_intern(symbol, intern_box);
    }
}

scm_object simple_read(FILE* file, scm_pair intern_box){
    char first_c;
    while (space_p(first_c = getc(file)));
    ungetc(first_c, file);

    if (first_c == '('){
        return simple_read_list(file, intern_box);
    }else{
        return simple_read_non_pair(file, intern_box);
    }
}

int plus_ope_p(scm_object obj){
    if (symbol_p(obj)){
        char* val = (char*)obj->value;
        return val[0] == '+' && val[1] == '\0';
    }
    return 0;
}

int mul_ope_p(scm_object obj){
    if (symbol_p(obj)){
        char* val = (char*)obj->value;
        return val[0] == '*' && val[1] == '\0';
    }
    return 0;
}

scm_object simple_calc(scm_object expression){
    if (pair_p(expression)){
        scm_object ope = ref_car(expression);
        scm_object args = ref_cdr(expression);
        if (mul_ope_p(ope)){
            int res_val = 1;
            while (!null_p(args)){
                res_val *= (simple_calc(ref_car(args)))->value;
                args = ref_cdr(args);
            }
            return make_fixnum(res_val);
        }else if (plus_ope_p(ope)){
            int res_val = 0;
            while (!null_p(args)){
                res_val += (simple_calc(ref_car(args)))->value;
                args = ref_cdr(args);
            }
            return make_fixnum(res_val);
        }else{
            return make_fixnum(0);
        }
    }else if (fixnum_p(expression)){
        return expression;
    }
    return make_fixnum(0);
}

void boot(scm_pair intern_box){
    symbol_intern(make_const_symbol("cons"), intern_box);
    symbol_intern(make_const_symbol("car"), intern_box);
    symbol_intern(make_const_symbol("cdr"), intern_box);
    symbol_intern(make_const_symbol("set-car!"), intern_box);
    symbol_intern(make_const_symbol("set-cdr!"), intern_box);
}

int main(void){
    printf("Hello cactus.\n");

    scm_pair intern_box = make_pair(null_object, null_object);
    boot(intern_box);

    scm_object input = simple_read(stdin, intern_box);
    //scm_object evaled = simple_calc(input);
    //simple_write(stdout,evaled);
    struct cactus_runtime_controller_t controller;

    scm_object x = make_pair( make_const_symbol("OK1"), make_const_symbol("OK2"));
    scm_object y = make_primitive(&cact_cdr);
    scm_object z =  call_scm_primitive(&controller, y, 1 ,x);
    simple_write(stdout,x); printf("\n");
    simple_write(stdout,y); printf("\n");
    simple_write(stdout,z); printf("\n");
    return 0;
}
