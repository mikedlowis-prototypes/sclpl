#include <sclpl.h>

char* ARGV0;
bool Verbose   = false;
char* Artifact = "bin";

/* Driver Modes
 *****************************************************************************/
static int emit_tokens(void) {
    Tok* token = NULL;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (token = gettoken(ctx)))
        pprint_token(stdout, token, true);
    return 0;
}

static int emit_tree(void) {
    AST* tree = NULL;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (tree = toplevel(ctx)))
        pprint_tree(stdout, tree, 0);
    return 0;
}

static int emit_csource(void) {
    return 0;
}

static int emit_object(void) {
    return 0;
}

static int emit_staticlib(void) {
    return 0;
}

static int emit_program(void) {
    return 0;
}

/* Main Routine and Usage
 *****************************************************************************/
void usage(void) {
    fprintf(stderr, "%s\n",
        "Usage: sclpl [OPTION]... MODE [FILE]...\n"
        "\n-A<type>   Emit the given type of artifact"
        "\n-h         Print help information"
        "\n-v         Enable verbose status messages");
    exit(1);
}

/* TODO:

    * Formalize grammar for parser
    * Paren for function application must be on same line as variable in REPL
    * Handle punctuation in the grammar directly instead of as an AST op.
    * Implement name mangling algorithm for files and identifiers

*/
int user_main(int argc, char **argv) {
    /* Option parsing */
    OPTBEGIN {
        case 'A': Artifact = EOPTARG(usage()); break;
        case 'v': Verbose = true; break;
        default:  usage();
    } OPTEND;

    /* Execute the main compiler process */
    if (0 == strcmp("bin", Artifact)) {
        return emit_program();
    } else if (0 == strcmp("lib", Artifact)) {
        return emit_staticlib();
    } else if (0 == strcmp("src", Artifact)) {
        return emit_csource();
    } else if (0 == strcmp("ast", Artifact)) {
        return emit_tree();
    } else if (0 == strcmp("tok", Artifact)) {
        return emit_tokens();
    } else {
        fprintf(stderr, "Unknonwn artifact type: '%s'\n\n", Artifact);
        usage();
    }
    return 1;
}

