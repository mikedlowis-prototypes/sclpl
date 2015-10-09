/**
  @file sclpl.h
*/
#ifndef SCLPL_H
#define SCLPL_H

#include <opts.h>
#include <libparse.h>
#include <stdio.h>
#include <stdarg.h>
#include "str.h"
//#include "vec.h"
#include "list.h"

typedef enum {
    TOKFILE,
    ASTFILE,
    CSOURCE,
    OBJECT,
    PROGRAM,
    STATICLIB,
    SHAREDLIB
} file_type_t;

/* Filesystem Routines */
str_t* sys_bin_dir(void);
str_t* sys_inc_dir(void);
str_t* sys_extension(file_type_t ftype);
str_t* sys_filename(file_type_t ftype, str_t* infile);

/* Pretty Printing Data Structures */
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
//void pprint_tree(FILE* file, AST* tree, int depth);

/* Compiler Driver Operations */
//vec_t* ops_parse_file(str_t* in);
//vec_t* ops_deps_file(vec_t* program);
str_t* ops_token_file(str_t* in);
//str_t* ops_syntax_file(str_t* in);
//str_t* ops_translate_file(str_t* in);
//str_t* ops_compile_file(str_t* in);

/* Error Logging */
void log_error(const char msg[], ...);

/* Code Generation */
void codegen_csource(FILE* file, vec_t* program);

#endif /* SCLPL_H */
