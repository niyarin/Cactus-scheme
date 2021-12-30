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
typedef scm_object ScmObject;
typedef struct pair_cell_t{
    void* car;
    void* cdr;
} *pair_cell;

typedef struct closure{
    ScmObject formals;
    ScmObject body;
    ScmObject env;
} *Closure;

typedef struct ephemeron_cell_t{
    void* key;
    void* datum;
    char broken;
} *ephemeron_cell;

typedef struct vector_info {
    int length;
    ScmObject* vector_container;
} *VectorInfo;

typedef struct port_info {
    int type;
    void* body;
} *PortInfo;

extern scm_object null_object;
extern scm_object true_object;
extern scm_object false_object;

extern ScmObject syntax_quote_object;
extern ScmObject syntax_lambda_object;
extern ScmObject syntax_define_object;
extern ScmObject syntax_if_object;
extern ScmObject syntax_set_object;
extern ScmObject syntax_pass_values_object;
extern ScmObject syntax_call_cc_internal_object;
extern ScmObject syntax_call_with_values_internal_object;

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

    scm_object local_stack;

    //namespace
    scm_object global;
    scm_object macro_env;
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
#define TYPE_CLOSURE 8
#define TYPE_VECTOR 9
#define TYPE_CONTINUATION 10
#define TYPE_PORT 11
#define TYPE_CHAR 12
#define TYPE_STRING 13

#define SYNTAX_QUOTE_ID 0
#define SYNTAX_LAMBDA_ID 1
#define SYNTAX_DEFINE_ID 2
#define SYNTAX_IF_ID 3
#define SYNTAX_SET_ID 4
#define SYNTAX_PASS_VALUES_ID 5
#define SYNTAX_CALL_CC_INTERNAL_ID 6
#define SYNTAX_CALL_WITH_VALUES_INTERNAL_ID 7
#define SYNTAX_DYNAMIC_WIND_INTERNAL_ID 8

#define PORT_FILE_OUTPUT_ID 0

scm_object make_scm_object(cactus_runtime_controller controller, char type, uintptr_t ptr);
scm_object make_const_scm_object(cactus_runtime_controller controller, char type, uintptr_t value);

#define ref_object_type(o) (o->type)
#define ref_object_value(o) (o->value)

scm_object make_fixnum(cactus_runtime_controller controller,intptr_t c_int);
int fixnum_p(scm_object object);
int built_in_syntax_p(scm_object object);

//pair
scm_object make_pair(cactus_runtime_controller controller, scm_object car, scm_object cdr);
scm_object make_list2(cactus_runtime_controller controller, ScmObject obj1, ScmObject obj2);
scm_object ref_car(scm_object pair);
#define ref_cadr(pair) ref_car(ref_cdr(pair))
scm_object ref_cdr(scm_object pair);
void set_car(scm_object pair, scm_object obj);
void set_cdr(scm_object pair, scm_object obj);
int pair_p(scm_object object);
scm_object assq(scm_list alist, scm_object key);

//ephemeron
scm_object make_ephemeron(cactus_runtime_controller controller, scm_object key, scm_object datum);
scm_object ephemeron_key(scm_ephemeron ephemeron);
scm_object ephemeron_datum(scm_ephemeron ephemeron);
int ephemeron_p(scm_object object);
void ephemeron_break(scm_ephemeron ephemeron);
int ephemeron_broken_p(scm_ephemeron ephemeron);

//vector
ScmObject make_vector(cactus_runtime_controller controller, size_t length);
ScmObject vector_ref(ScmObject vector, int index);
void vector_set(ScmObject vector, int index, ScmObject tgt);
int vector_length(ScmObject vector);

//char
ScmObject make_char(cactus_runtime_controller controller, uint32_t c);

//string
ScmObject make_string(cactus_runtime_controller controller,char* cstr);
int string_p(scm_object object);
char* string_to_cstr(ScmObject string);

//compare
ScmObject eq(ScmObject a, ScmObject b);

scm_object make_symbol(cactus_runtime_controller controller, char* c_str);
scm_object make_const_symbol(cactus_runtime_controller controller, char* c_str);
int symbol_p(scm_object object);
int simple_symbol_eq(scm_object symbol1, scm_object symbol2);
scm_object symbol_intern(cactus_runtime_controller controller, scm_symbol symbol);

scm_object make_primitive(cactus_runtime_controller controller, primitive_procedure fn);
int primitive_p(scm_object object);

ScmObject make_closure(cactus_runtime_controller controller, ScmObject args,ScmObject body ,ScmObject env);
int closure_p(scm_object object);
ScmObject ref_closure_body(ScmObject closure);
ScmObject ref_closure_formals(ScmObject closure);
int continuation_p(ScmObject object);
ScmObject ref_continuation_eval_stack(ScmObject continuation);

int null_p(scm_object object);

//port
ScmObject make_output_file_port(cactus_runtime_controller controller, FILE* file);
int output_port_p(scm_object object);

scm_object call_scm_primitive(cactus_runtime_controller controller, scm_primitive primitive, int n_args, ...);
scm_object cact_cons(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_car(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
scm_object cact_set_cdr(cactus_runtime_controller controller, int n_args,scm_object *arg_array);
ScmObject cact_pair_p(cactus_runtime_controller controller, int n_args,ScmObject *arg_array);

ScmObject make_continuation(cactus_runtime_controller controller);

ScmObject cact_make_vector(cactus_runtime_controller controller, int n_args,ScmObject *arg_array);

scm_object cact_eq(cactus_runtime_controller controller, int n_args,scm_object *arg_array);

ScmObject cact_call_cc(cactus_runtime_controller controller, int n_args,ScmObject *arg_array);
ScmObject cact_call_with_values(cactus_runtime_controller controller, int n_args, ScmObject *arg_array);

ScmObject cact_write_char(cactus_runtime_controller controller, int n_args, ScmObject *arg_array);

void add_all_objects(cactus_runtime_controller controller, scm_object object);
void gc_add_root(cactus_runtime_controller controller, scm_object object);
void gc(cactus_runtime_controller controller);

//utils
scm_object copy_list(cactus_runtime_controller controller, scm_object list);
scm_object list_reverse(cactus_runtime_controller controller, scm_object list);
int list_length(scm_object list);

//eval
ScmObject apply(cactus_runtime_controller controller, ScmObject procedure, ScmObject args);
scm_object eval(cactus_runtime_controller controller, scm_object expression);
scm_object pass_value(cactus_runtime_controller controller, scm_object value);
ScmObject call_eval_step(cactus_runtime_controller controller, scm_object solved_syntax_expression);
ScmObject make_eval_stack_cell(cactus_runtime_controller controller, ScmObject expression, ScmObject evaled, ScmObject un_evaled, ScmObject local_stack);

void add_global(cactus_runtime_controller controller, scm_symbol var, scm_object val);
scm_object lookup(cactus_runtime_controller controller, scm_symbol var);
void update(cactus_runtime_controller controller, scm_symbol var, ScmObject val);

scm_syntax lookup_syntax(cactus_runtime_controller controller,scm_symbol var);

void add_global_syntax(cactus_runtime_controller controller, scm_symbol var, scm_object val);


void boot(cactus_runtime_controller controller);

//to replace
scm_object simple_read(FILE* file, cactus_runtime_controller controller);
void simple_write(FILE *file, scm_object obj);
#endif
