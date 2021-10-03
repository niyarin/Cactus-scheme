#include<stdlib.h>
#include "cactus.h"

struct scm_object_t null_entity = {TYPE_NULL, 1, 0, 0};
struct scm_object_t true_entity = {TYPE_BOOLEAN, 1, 0, 1};
struct scm_object_t false_entity = {TYPE_BOOLEAN, 1, 0, 0};

struct scm_object_t quote_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_QUOTE_ID};
struct scm_object_t lambda_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_LAMBDA_ID};
struct scm_object_t define_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_DEFINE_ID};
struct scm_object_t if_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_IF_ID};
struct scm_object_t set_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_SET_ID};
struct scm_object_t pass_values_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_PASS_VALUES_ID};
struct scm_object_t call_cc_internal_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_CALL_CC_INTERNAL_ID};
struct scm_object_t call_with_values_internal_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_CALL_WITH_VALUES_INTERNAL_ID};
struct scm_object_t dynamic_wind_internal_syntax_entity = {TYPE_BUILT_IN_SYNTAX, 1, 0, SYNTAX_DYNAMIC_WIND_INTERNAL_ID};

ScmObject null_object = (scm_object)&null_entity;
ScmObject true_object = (scm_object)&true_entity;
ScmObject false_object = (scm_object)&false_entity;
ScmObject syntax_quote_object = (scm_object)&quote_syntax_entity;
ScmObject syntax_lambda_object = (scm_object)&lambda_syntax_entity;
ScmObject syntax_define_object = (ScmObject)&define_syntax_entity;
ScmObject syntax_if_object = (ScmObject)&if_syntax_entity;
ScmObject syntax_set_object = (ScmObject)&set_syntax_entity;
ScmObject syntax_pass_values_object = (ScmObject)&pass_values_syntax_entity;
ScmObject syntax_call_cc_internal_object = (ScmObject)&call_cc_internal_syntax_entity;
ScmObject syntax_call_with_values_internal_object = (ScmObject)&call_with_values_internal_syntax_entity;

scm_object make_scm_object(cactus_runtime_controller controller, char type, uintptr_t ptr){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res-> value_is_not_reference = 0;
   res->value = ptr;
   add_all_objects(controller,res);
   return res;
}

scm_object make_const_scm_object(cactus_runtime_controller controller, char type, uintptr_t value){
   scm_object res = (scm_object)malloc(sizeof(struct scm_object_t));
   res->type = type;
   res-> value_is_not_reference = 1;
   res->value = value;
   add_all_objects(controller,res);
   return res;
}

scm_object make_fixnum(cactus_runtime_controller controller,intptr_t c_int){
    return make_const_scm_object(controller, TYPE_FIX_NUM, c_int);
}

int fixnum_p(scm_object object){
    return object->type == TYPE_FIX_NUM;
}

int null_p(scm_object object){
    return object == null_object;
}

int built_in_syntax_p(scm_object object){
    return ref_object_type(object) == TYPE_BUILT_IN_SYNTAX;
}

int syntax_p(scm_object object){
    return built_in_syntax_p(object);
}

ScmObject make_output_file_port(cactus_runtime_controller controller, FILE* file){
    PortInfo port_info = (PortInfo)malloc(sizeof(struct port_info));
    port_info->body = file;
    port_info->type = PORT_FILE_OUTPUT_ID;
    return make_scm_object(controller, TYPE_PORT, (uintptr_t)port_info);
}

int output_port_p(scm_object object){
    PortInfo port_info = (PortInfo)ref_object_value(object);
    return ref_object_type(object) == TYPE_PORT
        && port_info->type == PORT_FILE_OUTPUT_ID;
}
