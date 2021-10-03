#ifndef CACTUS_H
#define CACTUS_H

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

typedef struct scm_object_t{
    uint8_t type;
    uint8_t value_is_not_reference;
    uint8_t mark;
    uintptr_t value;
} *scm_object;

#define scm_pair scm_object
#define scm_list scm_object
#define scm_symbol scm_object
#define scm_non_interned_symbol scm_object
#define scm_primitive scm_object
#define scm_ephemeron scm_object
#define scm_syntax scm_object

typedef struct pair_cell_t{
    void* car;
    void* cdr;
} *pair_cell;


typedef struct ephemeron_cell_t{
    void* key;
    void* datum;
    char broken;
} *ephemeron_cell;

extern scm_object null_object;
extern scm_object true_object;
extern scm_object false_object;

extern scm_object syntax_quote_object;

typedef struct cactus_runtime_controller_t{
    scm_pair symbol_intern;

    scm_object *all_objects;
    size_t all_objects_area_size;
    size_t all_objects_size;
    scm_list gc_roots;

    scm_list *ephemeron_queue;
    size_t ephemeron_queue_area_size;
    size_t ephemeron_queue_size;

    scm_object eval_stack;
    scm_object evaled,un_evaled;

    //namespace
    scm_object global;
    scm_object macro_env;
    size_t n_values;
    scm_object values;
} *cactus_runtime_controller;

typedef scm_object (*primitive_procedure)(cactus_runtime_controller,int, scm_object*);

#define TYPE_PAIR 0
#define TYPE_FIX_NUM 1
#define TYPE_SYMBOL 2
#define TYPE_NULL 3
#define TYPE_PRIMITIVE 4
#define TYPE_BOOLEAN 5
#define TYPE_EPHEMERON 6
#define TYPE_BUILT_IN_SYNTAX 7

#define SYNTAX_QUOTE_ID 0


scm_object make_scm_object(cactus_runtime_controller controller, char type, uintptr_t ptr);
scm_object make_const_scm_object(cactus_runtime_controller controller, char type, uintptr_t value);

#define ref_object_type(o) (o->type)
#define ref_object_value(o) (o->value)

scm_object make_fixnum(cactus_runtime_controller controller,intptr_t c_int);
int fixnum_p(scm_object object);

//pair
scm_object make_pair(cactus_runtime_controller controller, scm_object car, scm_object cdr);
scm_object ref_car(scm_object pair);
scm_object ref_cdr(scm_object pair);
void set_car(scm_object pair, scm_object obj);
void set_cdr(scm_object pair, scm_object obj);
int pair_p(scm_object object);
scm_object assq(scm_list alist, scm_object key);


scm_object make_ephemeron(cactus_runtime_controller controller, scm_object key, scm_object datum);
scm_object ephemeron_key(scm_ephemeron ephemeron);
scm_object ephemeron_datum(scm_ephemeron ephemeron);
int ephemeron_p(scm_object object);
void ephemeron_break(scm_ephemeron ephemeron);
int ephemeron_broken_p(scm_ephemeron ephemeron);


scm_object make_symbol(cactus_runtime_controller controller, char* c_str);
scm_object make_const_symbol(cactus_runtime_controller controller, char* c_str);
int symbol_p(scm_object object);
int simple_symbol_eq(scm_object symbol1, scm_object symbol2);
scm_object symbol_intern(cactus_runtime_controller controller, scm_symbol symbol);

scm_object make_primitive(cactus_runtime_controller controller, primitive_procedure fn);
int primitive_p(scm_object object);

int null_p(scm_object object);

scm_object call_scm_primitive(cactus_runtime_controller controller, scm_primitive primitive, int n_args, ...);
scm_object cact_cons(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);

void add_all_objects(cactus_runtime_controller controller, scm_object object);
void gc_add_root(cactus_runtime_controller controller, scm_object object);
void gc(cactus_runtime_controller controller);

//eval
scm_object eval(cactus_runtime_controller controller, scm_object expression);

void add_global(cactus_runtime_controller controller, scm_symbol var, scm_object val);
scm_object lookup(cactus_runtime_controller controller, scm_symbol var);

scm_syntax lookup_syntax(cactus_runtime_controller controller,scm_symbol var, scm_object val);
void add_global_syntax(cactus_runtime_controller controller, scm_symbol var, scm_object val);

//to replace
scm_object simple_read(FILE* file, cactus_runtime_controller controller);
#endif
