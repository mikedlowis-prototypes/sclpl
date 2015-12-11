#include <sclpl.h>

char* ARGV0;
bool verbose   = false;
char* artifact = "bin";

/* Command Line Options
 *****************************************************************************/
const char Usage[] =
    "Usage: sclpl [OPTION]... MODE [FILE]...\n"
    "\n-A<type>   Emit the given type of artifact"
    "\n-h         Print help information"
    "\n-v         Enable verbose status messages"
    "\n"
;

void usage(void) {
    fprintf(stderr, "%s", Usage);
    exit(1);
}

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

/* TODO:

    * Formalize grammar for parser
    * Paren for function application must be on same line as variable in REPL
    * Handle punctuation in the grammar directly instead of as an AST op.
    * Implement name mangling algorithm for files and identifiers

*/
int user_main(int argc, char **argv) {
    OPTBEGIN {
        case 'A': artifact = EOPTARG(usage()); break;
        case 'v': verbose = true; break;
        default:  usage();
    } OPTEND;

    /* Execute the main compiler process */
    if (0 == strcmp("bin", artifact)) {
        return emit_program();
    } else if (0 == strcmp("lib", artifact)) {
        return emit_staticlib();
    } else if (0 == strcmp("src", artifact)) {
        return emit_csource();
    } else if (0 == strcmp("ast", artifact)) {
        return emit_tree();
    } else if (0 == strcmp("tok", artifact)) {
        return emit_tokens();
    } else {
        fprintf(stderr, "Unknonwn artifact type: '%s'\n\n", artifact);
        usage();
    }
    return 1;
}

