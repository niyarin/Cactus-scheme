#include "cactus.h"
#include <assert.h>

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
    }else if (obj->type == TYPE_IDENTIFIER){
        simple_write(file, ((Identifier)obj->value)->symbol);
    }else if (obj->type == TYPE_NULL){
        fprintf(file, "nil");
    }else if (obj->type == TYPE_PRIMITIVE){
        fprintf(file, "#<primitive-procedure>");
    }else if (obj->type == TYPE_CLOSURE){
        fprintf(file, "#<closure>");
    }else if (obj->type == TYPE_CONTINUATION){
        fprintf(file, "#<continuation>");
    }else if (obj->type == TYPE_VECTOR){
        fprintf(file, "#(");
        for (int i=0;i<vector_length(obj);i++){
            simple_write(stdout, vector_ref(obj,i));
            fprintf(file, " ");
        }
        fprintf(file, ")");
    }else if (obj->type == TYPE_STRING){
        fprintf(file, "\"");
        int i=0;
        uint32_t *buff = (uint32_t*)ref_object_value(obj);
        while (buff[i]){
            char c = buff[i];
            fprintf(file,"%c",c);
            i++;
        }
        fprintf(file, "\"");
    }else if (obj->type == TYPE_BUILT_IN_SYNTAX){
        if (obj == syntax_quote_object){
            fprintf(file, "#<QUOTE>");
        }else if (obj == syntax_lambda_object){
            fprintf(file, "#<LAMBDA>");
        }else{
            fprintf(file, "#<SYNTAX>");
        }
    }else if (obj == true_object){
        fprintf(file, "#t");
    }else if (obj == false_object){
        fprintf(file, "#f");
    }else if (obj->type == TYPE_CHAR){
        if (obj->value < 128){
            fprintf(file, "#\\%c",obj->value);
        }
    }else if (obj == false_object){
    }else{
        fprintf(file, "#<??>");
    }
}

void write_char(cactus_runtime_controller controller, ScmObject c, ScmObject port){
    PortInfo port_info = ref_object_value(port);

    if (port_info->type == PORT_FILE_OUTPUT_ID){
        fputc(ref_object_value(c), port_info->body);
    }else{
        assert(0);
    }
}

