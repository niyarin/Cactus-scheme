#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "cactus.h"

#define RUN_MODE_REPL 0
#define RUN_MODE_SCRIPT 1
#define RUN_MODE_PRINT_HELP 2
#define RUN_MODE_PRINT_VERSION 3

#define CACTUS_SCHEME_VERSION "alpha"

int parse_optparse(int argc, char **argv){
    char mode = RUN_MODE_REPL;
    int argv_pos = 1;
    while (argv_pos < argc){
        if (strcmp(argv[argv_pos],"-l")==0){
            //ADD LOAD PATH
        }else if (strcmp(argv[argv_pos],"--help")==0 ||
                  strcmp(argv[argv_pos],"-h")==0){
            mode = RUN_MODE_PRINT_HELP;
            return mode;
            //PRINT HELP
        }else if (strcmp(argv[argv_pos],"--version")==0||
                  strcmp(argv[argv_pos],"-h")==0){
            //PRINT VERSION
            mode = RUN_MODE_PRINT_VERSION;
            return mode;
        }else if (argv[argv_pos][0]){
        }else {
        }

        argv_pos++;
    }
    return mode;
}

void print_help(){
    printf("cactus-scheme [options] file...\n Options:\n\
            --help, -h  Display this information\n\
            --version, -v Display version\n\
            -l <path> Add load path\n");
}

void print_version(){
    printf("Cactus-scheme, version %s\n",CACTUS_SCHEME_VERSION);
}

void repl(cactus_runtime_controller controller){//echo only
    simple_write(stdout, controller->macro_env);
    printf("\n");
    while (1){
        printf("> ");
        scm_object input = simple_read(stdin, controller);
        scm_object res = eval(controller, input);
        simple_write(stdout, res);printf("\n");
    }
}

int main(int argc, char **argv){
    int run_mode = parse_optparse(argc, argv);
    if (run_mode == RUN_MODE_PRINT_HELP){
        print_help();
        return 0;
    }else if (run_mode == RUN_MODE_PRINT_VERSION){
        print_version();
        return 0;
    }

    struct cactus_runtime_controller_t controller;
    boot(&controller);

    /*
    scm_object input = simple_read(stdin, &controller);
    simple_write(stdout, input);printf("\n");
    simple_write(stdout, true_object);printf("\n");
    simple_write(stdout, false_object);printf("\n");

    scm_object dead = make_pair(&controller, make_const_symbol(&controller,"ABC"),make_const_symbol(&controller,"EFG"));

    scm_object piyo = make_const_symbol(&controller,"EFG");
    scm_object ephemeron = make_ephemeron(&controller,piyo,piyo);

    gc_add_root(&controller, ephemeron);
    gc_add_root(&controller, input);
    gc_add_root(&controller, controller.symbol_intern);

    gc(&controller);
    printf("%ld\n", ephemeron_broken_p(ephemeron));
    */

    if (run_mode == RUN_MODE_REPL){
        repl(&controller);
    }
    return 0;
}
