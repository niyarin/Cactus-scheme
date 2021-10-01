#ifndef CACTUS_H
#define CACTUS_H

#include<stdint.h>
#include<stdarg.h>

typedef struct scm_object_t{
    uint8_t type;
    uint8_t value_is_not_reference;
    uintptr_t value;
} *scm_object;

#define scm_pair scm_object
#define scm_list scm_object
#define scm_symbol scm_object
#define scm_non_interned_symbol scm_object
#define scm_primitive scm_object


typedef struct pair_cell_t{
    void* car;
    void* cdr;
} *pair_cell;

extern scm_object null_object;

typedef struct cactus_runtime_controller_t{
    scm_pair symbol_intern;
} *cactus_runtime_controller;

typedef scm_object (*primitive_procedure)(cactus_runtime_controller,int, scm_object*);

#define TYPE_PAIR 0
#define TYPE_FIX_NUM 1
#define TYPE_SYMBOL 2
#define TYPE_NULL 3
#define TYPE_PRIMITIVE 4

scm_object make_scm_object(char type, uintptr_t ptr);
scm_object make_const_scm_object(char type, uintptr_t value);

#define ref_object_type(o) (o->type)
#define ref_object_value(o) (o->value)

scm_object make_fixnum(intptr_t c_int);
int fixnum_p(scm_object object);

scm_object make_pair(scm_object car, scm_object cdr);
scm_object ref_car(scm_object pair);
scm_object ref_cdr(scm_object pair);
void set_car(scm_object pair, scm_object obj);
void set_cdr(scm_object pair, scm_object obj);
int pair_p(scm_object object);

scm_object make_symbol(char* c_str);
scm_object make_const_symbol(char* c_str);
int symbol_p(scm_object object);
int simple_symbol_eq(scm_object symbol1, scm_object symbol2);
scm_object symbol_intern(scm_symbol symbol, scm_pair intern_box);

scm_object make_primitive(primitive_procedure fn);
int primitive_p(scm_object object);

int null_p(scm_object object);

scm_object call_scm_primitive(cactus_runtime_controller controller, scm_primitive primitive, int n_args, ...);
scm_object cact_cons(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);

#endif
