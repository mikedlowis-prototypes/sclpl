#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "opts.h"
#include "grammar.h"
#include "parser.h"
#include "lexer.h"
#include "pprint.h"

/* Command Line Options
 *****************************************************************************/
opts_cfg_t Options_Config[] = {
    {"tokens",    false, "mode",    "Emit the token output of lexical analysis for the given file"},
    {"ast",       false, "mode",    "Emit the abstract syntax tree for the given file"},
    {"csource",   false, "mode",    "Emit the intermediate C source file for the given file"},
    {"repl",      false, "mode",    "Execute the application in a REPL"},
    {"staticlib", false, "mode",    "Compile the application as a static library"},
    {"sharedlib", false, "mode",    "Compile the application as a shared library"},
    {"program",   false, "mode",    "Compile the application as an executable"},
    {"e",         true,  "entry",   "Specify the module that contains the entry point for the executable"},
    {"entry",     true,  "entry",   "Specify the module that contains the entry point for the executable"},
    {"R",         true,  "include", "Add a path to the list of require paths"},
    {NULL,        false, NULL,      NULL }
};

void print_usage(void) {
    opts_cfg_t* opts = &Options_Config[0];
    bool opts_have_args = false;
    size_t sz = 0;
    /* Figure out the longest option name */
    while (NULL != opts->name) {
        size_t name_sz = strlen(opts->name);
        if (name_sz > sz) {
            sz = name_sz;
        }
        if (opts->has_arg) {
            opts_have_args = true;
        }
        opts++;
    }

    /* Print the usage and option list */
    printf("Usage: [OPTION]... [MODE] [FILE]\n\n");
    size_t padding = sz + 4 + ((opts_have_args) ? 4 : 0);
    char*  buffer  = (char*)malloc(padding+1);
    opts = &Options_Config[0];
    while (NULL != opts->name) {
        if (1 == strlen(opts->name))
            sprintf(buffer, " -%s", opts->name);
        else
            sprintf(buffer, " --%s", opts->name);
        if (opts->has_arg) sprintf(&buffer[strlen(buffer)], "=ARG ");
        printf("%-*s%s\n", padding, buffer, opts->desc);
        opts++;
    }
    free(buffer);

    exit(1);
}

/* Tree Rewriting
 *****************************************************************************/
bool is_punctuation(lex_tok_t* p_tok) {
    bool ret = false;
    switch(p_tok->type) {
        case T_END:
        case T_LBRACE:
        case T_RBRACE:
        case T_LBRACK:
        case T_RBRACK:
        case T_LPAR:
        case T_RPAR:
        case T_COMMA:
            ret = true;
        default:
            break;
    }
    return ret;
}

tree_t* convert_to_ast(tree_t* p_tree) {
    tree_t* p_newtree = NULL;
    if (p_tree->tag == ATOM) {
        if (!is_punctuation(p_tree->ptr.tok))
            p_newtree = mem_retain(p_tree);
    } else {
        vec_t* p_vec = p_tree->ptr.vec;
        vec_t* p_newvec = vec_new(0);
        p_newtree = parser_tree_new(TREE, p_newvec);
        for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
            tree_t* p_item = convert_to_ast(vec_at(p_vec,idx));
            if (NULL != p_item)
                vec_push_back(p_newvec, p_item);
        }
    }
    return p_newtree;
}

/* Driver Modes
 *****************************************************************************/
static int emit_tokens(void) {
    lexer_t* p_lexer = lexer_new(NULL, stdin);
    lex_tok_t* token;
    while(NULL != (token = lexer_read(p_lexer))) {
        pprint_token(stdout, token);
        mem_release(token);
    }
    mem_release(p_lexer);
    return 0;
}

static int emit_tree(void) {
    int ret = 0;
    parser_t* p_parser = parser_new(NULL, stdin);
    while(!parser_eof(p_parser)) {
        tree_t* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            tree_t* p_ast = convert_to_ast(p_tree);
            pprint_tree(stdout, p_ast, 0);
            mem_release(p_tree);
            mem_release(p_ast);
        } else {
            parser_resume(p_parser);
            ret = 1;
        }
    }
    mem_release(p_parser);
    return ret;
}

static int emit_csource(void) {
    int ret = 0;
    parser_t* p_parser = parser_new(NULL, stdin);
    vec_t* p_vec = vec_new(0);
    while(!parser_eof(p_parser)) {
        tree_t* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            tree_t* p_ast = convert_to_ast(p_tree);
            mem_release(p_tree);
            vec_push_back(p_vec, p_ast);
        } else {
            parser_resume(p_parser);
            ret = 1;
        }
    }
    if (0 == ret)
        codegen_csource(stdout, p_vec);
    mem_release(p_vec);
    mem_release(p_parser);
    return ret;
}

static int exec_repl(void) {
    parser_t* p_parser = parser_new(":> ", stdin);
    while(!parser_eof(p_parser)) {
        tree_t* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            tree_t* p_ast = convert_to_ast(p_tree);
            pprint_tree(stdout, p_ast, 0);
            mem_release(p_tree);
            mem_release(p_ast);
            puts("OK.");
        } else {
            parser_resume(p_parser);
        }
    }
    mem_release(p_parser);
    return 0;
}

static int emit_staticlib(void) {
    return 0;
}

static int emit_sharedlib(void) {
    return 0;
}

static int emit_program(void) {
    return 0;
}

/* TODO:

    * Formalize grammar for parser
    * Paren for function application must be on same line as variable in REPL
    * Handle punctuation in the grammar directly instead of as an AST op.
    * Implement name mangling algorithm for files and identifiers

*/
int main(int argc, char **argv) {
    opts_parse( Options_Config, argc, argv );

    if (!opts_is_set(NULL,"mode")) {
        print_usage();
    } else if(opts_equal(NULL, "mode", "repl")) {
        return exec_repl();
    } else if (opts_equal(NULL, "mode", "tokens")) {
        return emit_tokens();
    } else if (opts_equal(NULL, "mode", "ast")) {
        return emit_tree();
    } else if (opts_equal(NULL, "mode", "csource")) {
        return emit_csource();
    } else if (opts_equal(NULL, "mode", "staticlib")) {
        return emit_staticlib();
    } else if (opts_equal(NULL, "mode", "sharedlib")) {
        return emit_sharedlib();
    } else if (opts_equal(NULL, "mode", "program")) {
        return emit_program();
    } else {
        print_usage();
    }

    opts_reset();
    return 1;
}
