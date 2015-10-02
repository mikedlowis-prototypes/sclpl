/**
  @file opts.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "opts.h"
#include "parser.h"
#include "str.h"
#include "sys.h"
#include "tree.h"
#include "grammar.h"
#include "pprint.h"
#include "codegen.h"

vec_t* ops_parse_file(str_t* in) {
    bool failed = false;
    FILE* input = (NULL == in) ? stdin : fopen(str_cstr(in), "r");
    Parser* p_parser = parser_new(NULL, input);
    vec_t* p_vec = vec_new(0);
    while(!parser_eof(p_parser)) {
        AST* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            AST* p_ast = tree_convert(p_tree);
            mem_release(p_tree);
            vec_push_back(p_vec, p_ast);
        } else {
            parser_resume(p_parser);
            failed = true;
        }
    }
    mem_release(p_parser);
    if (failed) mem_release(p_vec);
    return ((failed) ? NULL : p_vec);
}

vec_t* ops_deps_file(vec_t* program) {
    vec_t* deps = vec_new(0);
    (void)program;
    return deps;
}

str_t* ops_token_file(str_t* in) {
    str_t* ofname = NULL;
    FILE* input = (NULL == in) ? stdin : fopen(str_cstr(in), "r");
    FILE* output;
    if (NULL == in) {
        output = stdout;
    } else {
        ofname = sys_filename(TOKFILE, in);
        output = fopen(str_cstr(ofname), "w");
    }

    Lexer* p_lexer = lexer_new(NULL, input);
    Token* token;
    while(NULL != (token = lexer_read(p_lexer))) {
        pprint_token(output, token, true);
        mem_release(token);
    }
    mem_release(p_lexer);

    return ofname;
}

str_t* ops_syntax_file(str_t* in) {
    str_t* ofname = NULL;
    FILE* output;
    if (NULL == in) {
        output = stdout;
    } else {
        ofname = sys_filename(ASTFILE, in);
        output = fopen(str_cstr(ofname), "w");
    }
    vec_t* program = ops_parse_file(in);
    if (NULL != program) {
        for (size_t idx = 0; idx < vec_size(program); idx++) {
            pprint_tree(output, (AST*)vec_at(program, idx), 0);
        }
        mem_release(program);
        fclose(output);
    } else {
        fclose(output);
        if (NULL != ofname)
            remove(str_cstr(ofname));
        mem_release(ofname);
        ofname = NULL;
    }
    return ofname;
}

str_t* ops_translate_file(str_t* in) {
    str_t* ofname = NULL;
    FILE* output;
    if (NULL == in) {
        output = stdout;
    } else {
        ofname = sys_filename(CSOURCE, in);
        output = fopen(str_cstr(ofname), "w");
    }
    vec_t* program = ops_parse_file(in);
    codegen_csource(output, program);
    fclose(output);
    mem_release(program);
    return ofname;
}

str_t* ops_compile_file(str_t* in) {
    str_t* ofname  = sys_filename(OBJECT, in);
    vec_t* parts   = vec_new(5, str_new("cc -c -o"), mem_retain(ofname), str_new("-I"), sys_inc_dir(), mem_retain(in));
    str_t* command = str_join(" ", parts);
    if (opts_is_set(NULL, "verbose"))
        puts(str_cstr(command));
    if (0 != system(str_cstr(command))) {
        remove(str_cstr(ofname));
        mem_swap((void**)&ofname, NULL);
    }
    remove(str_cstr(in));
    mem_release(parts);
    mem_release(command);
    return ofname;
}

