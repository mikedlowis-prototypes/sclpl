#include <sclpl.h>

char* ARGV0;
bool Verbose   = false;
char* Artifact = "bin";

bool isatomic(AST* tree)
{
    switch (tree->type) {
        case AST_STRING:
        case AST_SYMBOL:
        case AST_IDENT:
        case AST_CHAR:
        case AST_INT:
        case AST_FLOAT:
        case AST_BOOL:
        case AST_FUNC:
            return true;
        default:
            return false;
    }
}

//AST* normalize_fnapp_args(AST* tree)
//{
//}

AST* normalize_fnapp(AST* tree)
{
    AST* normalized = tree;
    AST* fn = fnapp_fn(tree);
    /* Normalize the function */
    if (!isatomic(fn)) {
        AST* temp = TempVar();
        fnapp_set_fn(tree, temp);
        normalized = Let(temp, fn, tree);
    }
    /* Normalize the function arguments */
    vec_t temps;
    vec_init(&temps);
    vec_t* args = fnapp_args(tree);
    for (int i = 0; i < vec_size(args); i++) {
        AST* arg = (AST*)vec_at(args, i);
        if (!isatomic(arg)) {
            AST* temp = TempVar();
            vec_push_back(&temps, Let(temp, arg, NULL));
            vec_set(args, i, temp);
        }
    }
    /* Nest all the scopes and return the new form */
    for (int i = vec_size(&temps); i > 0; i--) {
        AST* let = (AST*)vec_at(&temps,i-1);
        let_set_body(let, normalized);
        normalized = let;
    }
    vec_deinit(&temps);
    return normalized;
}

AST* normalize(AST* tree)
{
    if (tree->type == AST_FNAPP)
        return normalize_fnapp(tree);
    else
        return tree;
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

static int emit_ast(void) {
    AST* tree = NULL;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (tree = toplevel(ctx)))
        pprint_tree(stdout, tree, 0);
    return 0;
}

static int emit_anf(void) {
    AST* tree = NULL;
    Parser* ctx = parser_new(NULL, stdin);
    while(NULL != (tree = toplevel(ctx)))
        pprint_tree(stdout, normalize(tree), 0);
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
    } else if (0 == strcmp("anf", Artifact)) {
        return emit_anf();
    } else if (0 == strcmp("src", Artifact)) {
        return emit_csource();
    } else if (0 == strcmp("bin", Artifact)) {
        return emit_program();
    } else if (0 == strcmp("lib", Artifact)) {
        return emit_staticlib();
    } else {
        fprintf(stderr, "Unknonwn artifact type: '%s'\n\n", Artifact);
        usage();
    }
    return 1;
}

