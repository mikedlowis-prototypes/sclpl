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

static int emit_ast(void) {
    AST* tree = NULL;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (tree = toplevel(ctx)))
        pprint_tree(stdout, tree, 0);
    return 0;
}

static int emit_binary(void) {
    return 0;
}

/* Main Routine and Usage
 *****************************************************************************/
void usage(void) {
    fprintf(stderr, "%s\n",
        "Usage: sclpl [options...] [-A artifact] [file...]\n"
        "\n-A<artifact> Emit the given type of artifact"
        "\n-h           Print help information"
        "\n-v           Enable verbose status messages");
    exit(1);
}

int user_main(int argc, char **argv) {
    /* Option parsing */
    OPTBEGIN {
        case 'A': Artifact = EOPTARG(usage()); break;
        case 'v': Verbose = true; break;
        default:  usage();
    } OPTEND;

    /* Execute the main compiler process */

    if (0 == strcmp("tok", Artifact)) {
        return emit_tokens();
    } else if (0 == strcmp("ast", Artifact)) {
        return emit_ast();
    } else if (0 == strcmp("bin", Artifact)) {
        return emit_binary();
    } else {
        fprintf(stderr, "Unknonwn artifact type: '%s'\n\n", Artifact);
        usage();
    }
    return 1;
}

