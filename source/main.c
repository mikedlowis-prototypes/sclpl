#include <sclpl.h>

char* ARGV0;
char* Artifact = "ast";

/* Builtin Types
 *****************************************************************************/
static void builtins(Parser* p) {
    sym_addtype(&(p->syms), "void",   VoidType());
    sym_addtype(&(p->syms), "bool",   UIntType(1u));
    sym_addtype(&(p->syms), "byte",   UIntType(8u));
    sym_addtype(&(p->syms), "uint",   UIntType(64u));
    sym_addtype(&(p->syms), "u8",     UIntType(8u));
    sym_addtype(&(p->syms), "u16",    UIntType(16u));
    sym_addtype(&(p->syms), "u32",    UIntType(32u));
    sym_addtype(&(p->syms), "u64",    UIntType(64u));
    sym_addtype(&(p->syms), "int",    IntType(64u));
    sym_addtype(&(p->syms), "i8",     IntType(8u));
    sym_addtype(&(p->syms), "i16",    IntType(16u));
    sym_addtype(&(p->syms), "i32",    IntType(32u));
    sym_addtype(&(p->syms), "i64",    IntType(64u));
    sym_addtype(&(p->syms), "string",
        ArrayOf(sym_get(&(p->syms), "byte")->type, -1));
}

/* Driver Modes
 *****************************************************************************/
static int emit_tokens(void) {
    Tok token = { 0 };
    Parser ctx = { .input = stdin };
    while (1) {
        gettoken(&ctx, &token);
        if (token.type == T_END_FILE)
            break;
        else
            pprint_token(stdout, &token, true);
    }
    return 0;
}

static int emit_ast(void) {
    AST* tree = NULL;
    Parser ctx = { .input = stdin };
    builtins(&ctx);
    while(NULL != (tree = toplevel(&ctx)))
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
    );
    exit(1);
}

int main(int argc, char **argv) {
    /* Option parsing */
    OPTBEGIN {
        case 'A': Artifact = EOPTARG(usage()); break;
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
