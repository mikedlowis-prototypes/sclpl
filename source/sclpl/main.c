#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "opts.h"
#include "str.h"
#include "list.h"
#include "grammar.h"
#include "parser.h"
#include "lexer.h"
#include "pprint.h"
#include "codegen.h"

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
    {"entry",     true,  "entry",   "Specify the module that contains the entry point for the executable"},
    {"e",         true,  "entry",   "Specify the module that contains the entry point for the executable"},
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
    puts(Usage);
    int padding = sz + 4 + ((opts_have_args) ? 4 : 0);
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

void error_msg(const char msg[], ...) {
    va_list args;
    va_start(args, msg);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msg, args);
    va_end(args);
    fputs("\n",stderr);
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
            error_msg("no such file or directory: %s", files[0]);
            exit(1);
        }
        list_push_front(infiles, str_new(files[0]));
        files++;
    }
    mem_release(list_pop_front(infiles));
    free(fvec);
    return infiles;
}

/* Command Building
 *****************************************************************************/
char Object_Cmd[]    = "clang -c -I. -o %s %s";
char Program_Cmd[]   = "clang -o %s %s";
char StaticLib_Cmd[] = "ar rcs %s %s";
char SharedLib_Cmd[] = "clang -shared %s";

str_t* str_join(char* joinstr, vec_t* strs) {
    str_t* ret = str_new("");
    str_t* jstr = str_new(joinstr);
    for (size_t idx = 0; idx < vec_size(strs); idx++) {
        str_t* str = (str_t*)vec_at(strs, idx);
        if (str_size(ret) > 0)
            mem_swap((void**)&ret, str_concat(ret, jstr));
        mem_swap((void**)&ret, str_concat(ret, str));
    }
    mem_release(jstr);
    return ret;
}

/* Utility Functions
 *****************************************************************************/
typedef enum {
    CSOURCE,
    OBJECT,
    PROGRAM,
    STATICLIB,
    SHAREDLIB
} file_type_t;

str_t* get_extension(file_type_t ftype) {
    str_t* ext = NULL;
    switch (ftype) {
        case CSOURCE:   ext = str_new(".c");   break;
        case OBJECT:    ext = str_new(".o");   break;
        case PROGRAM:   ext = str_new("");     break;
        case STATICLIB: ext = str_new(".a");   break;
        case SHAREDLIB: ext = str_new(".lib"); break;
        default:        ext = str_new("");     break;
    }
    return ext;
}

str_t* get_filename(file_type_t ftype, str_t* infile) {
    str_t* ext_ind = str_new(".");
    size_t index   = str_rfind(infile, ext_ind);
    str_t* rawname = str_substr(infile, 0, index);
    str_t* ext = get_extension(ftype);
    str_t* fname = str_concat(rawname, ext);
    mem_release(ext_ind);
    mem_release(rawname);
    mem_release(ext);
    return fname;
}

vec_t* parse_file(str_t* in) {
    FILE* input = (NULL == in) ? stdin : fopen(str_cstr(in), "r");
    parser_t* p_parser = parser_new(NULL, input);
    vec_t* p_vec = vec_new(0);
    bool failed = false;
    while(!parser_eof(p_parser)) {
        tree_t* p_tree = grammar_toplevel(p_parser);
        if (NULL != p_tree) {
            tree_t* p_ast = convert_to_ast(p_tree);
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

str_t* translate_file(str_t* in) {
    str_t* ofname = NULL;
    FILE* output;
    if (NULL == in) {
        output = stdout;
    } else {
        ofname = get_filename(CSOURCE, in);
        output = fopen(str_cstr(ofname), "w");
    }
    vec_t* program = parse_file(in);
    codegen_csource(output, program);
    fclose(output);
    mem_release(program);
    return ofname;
}

str_t* compile_file(str_t* in) {
    str_t* ofname  = get_filename(OBJECT, in);
    vec_t* parts   = vec_new(3, str_new("clang -c -o"), mem_retain(ofname), mem_retain(in));
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

#if 0
str_t* link_files(list_t* in) {
    str_t* ofname = get_filename(get_output_type(), in);
    return ofname;
}
#endif

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
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        (void)translate_file(NULL);
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        mem_release( translate_file(fname) );
    } else {
        error_msg("too many files provided for target mode 'csource'");
    }
    mem_release(files);
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

static int emit_object(void) {
    list_t* files  = input_files();
    size_t  nfiles = list_size(files);
    if (0 == nfiles) {
        error_msg("too few files provided for target mode 'object'");
    } else if (1 == nfiles) {
        str_t* fname = list_front(files)->contents;
        str_t* csrc  = translate_file(fname);
        str_t* obj   = compile_file(csrc);
        mem_release(csrc);
        mem_release(obj);
    } else {
        error_msg("too many files provided for target mode 'object'");
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
