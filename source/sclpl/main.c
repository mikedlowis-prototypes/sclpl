#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "opts.h"
#include "str.h"
#include "list.h"
#include "pprint.h"
#include "codegen.h"
#include "sys.h"
#include "log.h"
#include "ops.h"
#include <libparse.h>


/* Command Line Options
 *****************************************************************************/
const char Usage[] = "Usage: sclpl [OPTION]... MODE [FILE]...\n";

opts_cfg_t Options_Config[] = {
    {"tokens",    false, "mode",    "Emit the token output of lexical analysis for the given file"},
    {"ast",       false, "mode",    "Emit the abstract syntax tree for the given file"},
    {"csource",   false, "mode",    "Emit the intermediate C source file for the given file"},
    {"repl",      false, "mode",    "Execute the application in a REPL"},
    {"object",    false, "mode",    "Compile the source as an object file"},
    {"staticlib", false, "mode",    "Compile the application as a static library"},
    {"sharedlib", false, "mode",    "Compile the application as a shared library"},
    {"program",   false, "mode",    "Compile the application as an executable"},
    {"verbose",   false, "verbose", "Enable verbose status messages"},
    {"v",         false, "verbose", "Enable verbose status messages"},
    {NULL,        false, NULL,      NULL }
};

void print_usage(void) {
    puts(Usage);
    opts_print_help(stdout, Options_Config);
    exit(1);
}

/* Options Helpers
 *****************************************************************************/
bool file_exists(const char* name) {
    bool  ret  = false;
    FILE* file = fopen(name,"r");
    if (NULL != file) {
        fclose(file);
        ret = true;
    }
    return ret;
}

list_t* input_files(void) {
    list_t* infiles = list_new();
    const char** fvec  = opts_arguments();
    const char** files = fvec;
    while (NULL != files[0]) {
        if (!file_exists(files[0])) {
            mem_release(infiles);
            log_error("no such file or directory: %s", files[0]);
            exit(1);
        }
        list_push_front(infiles, str_new(files[0]));
        files++;
    }
    free(fvec);
    return infiles;
}

/* Driver Modes
 *****************************************************************************/
static int emit_tokens(void) {
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        (void)ops_token_file(NULL);
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        mem_release( ops_token_file(fname) );
    } else {
        log_error("too many files provided for target mode 'tokens'");
    }
    mem_release(files);
    return 0;
}

static int emit_tree(void) {
    int ret = 0;
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        (void)ops_syntax_file(NULL);
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        mem_release( ops_syntax_file(fname) );
    } else {
        log_error("too many files provided for target mode 'ast'");
    }
    mem_release(files);

    return ret;
}

static int emit_csource(void) {
    int ret = 0;
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        (void)ops_translate_file(NULL);
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        mem_release( ops_translate_file(fname) );
    } else {
        log_error("too many files provided for target mode 'csource'");
    }
    mem_release(files);
    return ret;
}

static int exec_repl(void) {
    Parser* p_parser = parser_new(":> ", stdin);
    while(!parser_eof(p_parser)) {
        AST* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            AST* p_ast = tree_convert(p_tree);
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

static int emit_object(void) {
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        log_error("too few files provided for target mode 'object'");
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        str_t* csrc  = ops_translate_file(fname);
        str_t* obj   = ops_compile_file(csrc);
        mem_release(csrc);
        mem_release(obj);
    } else {
        log_error("too many files provided for target mode 'object'");
    }
    mem_release(files);
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
    } else if (opts_equal(NULL, "mode", "object")) {
        return emit_object();
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
