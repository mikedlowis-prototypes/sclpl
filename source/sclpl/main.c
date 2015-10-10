#include <sclpl.h>

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

/* Driver Modes
 *****************************************************************************/
static int emit_tokens(void) {
    Tok* token;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (token = gettoken(ctx)))
        pprint_token(stdout, token, true);
    return 0;
}

static int emit_tree(void) {
    return 0;
}

static int emit_csource(void) {
    return 0;
}

static int exec_repl(void) {
    return 0;
}

static int emit_object(void) {
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
    } else if (opts_equal(NULL, "mode", "tokens")) {
        return emit_tokens();
    } else if(opts_equal(NULL, "mode", "repl")) {
        return exec_repl();
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
