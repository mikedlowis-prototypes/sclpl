#include "mpc.h"
#include "ast.h"
#include <stdio.h>
#include <assert.h>

/* SCLPL Parser
 *****************************************************************************/
/* Grammar is auto generated into 'source/grammar.c' */
extern const char Grammar[];

static ast_t* read_sexpr(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_qexpr(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_char(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_string(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_var(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_bool(const mpc_ast_t* t) {
    (void)t;
    return NULL;
}

static ast_t* read_float(const mpc_ast_t* t) {
    double* p_dbl = (double*)malloc(sizeof(double));
    ast_t* p_ast = ast_new(FLOAT, p_dbl);
    ast_set_pos(p_ast, "<stdin>", t->state.row, t->state.col);
    errno = 0;
    *p_dbl = strtod(t->contents, NULL);
    assert(errno == 0);
    return p_ast;
}

static ast_t* read_int(const mpc_ast_t* t, int base) {
    long* p_int = (long*)malloc(sizeof(long));
    printf("reading int with base: %d\n", base);
    ast_t* p_ast = ast_new(INTEGER, p_int);
    ast_set_pos(p_ast, "<stdin>", t->state.row, t->state.col);
    errno = 0;
    *p_int = strtol(t->contents, NULL, base);
    assert(errno == 0);
    return p_ast;
}

static int read_radix(const mpc_ast_t* t) {
    switch( t->children[0]->contents[1] ) {
        case 'b': return 2;
        case 'o': return 8;
        case 'd': return 10;
        case 'x': return 16;
        default:  return 10;
    }
}

ast_t* format_expr_ast(mpc_ast_t* expr) {
    ast_t* p_ast = NULL;

    /* Handle the current node */
    if (0 == strcmp("sexpr|>", expr->tag)) {
    } else if (0 == strcmp("qexpr|>", expr->tag)) {
    } else if (0 == strcmp("radixnum|>", expr->tag)) {
        p_ast = read_int(expr->children[1], read_radix(expr));
        printf("int: %d\n", *((long*)p_ast->value));
    } else if (0 == strcmp("expr|float|regex", expr->tag)) {
        p_ast = read_float(expr);
        printf("double: %f\n", *((double*)p_ast->value));
    } else if (0 == strcmp("expr|int|regex", expr->tag)) {
        p_ast = read_int(expr,10);
        printf("int: %d\n", *((long*)p_ast->value));
    } else if (0 == strcmp("ch|>", expr->tag)) {
    } else if (0 == strcmp("str|>", expr->tag)) {
    } else if (0 == strcmp("expr|bool|str", expr->tag)) {
    } else if (0 == strcmp("expr|var|regex", expr->tag)) {
    } else {
        printf("unknown tag: '%s'\n", expr->tag);
        free(p_ast->pos);
        free(p_ast);
        p_ast = NULL;
    }

    return p_ast;
}

int main(int argc, char **argv) {
    mpc_parser_t* ReplExpr = mpc_new("replexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* SExpr = mpc_new("sexpr");
    mpc_parser_t* QExpr = mpc_new("qexpr");
    mpc_parser_t* Atom = mpc_new("atom");
    mpc_parser_t* Int = mpc_new("int");
    mpc_parser_t* Float = mpc_new("float");
    mpc_parser_t* Radix = mpc_new("radixnum");
    mpc_parser_t* Char = mpc_new("ch");
    mpc_parser_t* String = mpc_new("str");
    mpc_parser_t* Bool = mpc_new("bool");
    mpc_parser_t* Var = mpc_new("var");
    mpc_parser_t* WS = mpc_new("ws");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE, Grammar,
        ReplExpr, Expr, SExpr, QExpr, Atom, Int, Float, Radix, Char, String, Bool, Var, WS, NULL);
    while(!feof(stdin)) {
        mpc_result_t r;
        printf(":> ");
        if (mpc_parse_pipe("<stdin>", stdin, ReplExpr, &r)) {
            mpc_ast_t* expr = (mpc_ast_t*)(((mpc_ast_t*)r.output)->children[1]);
            mpc_ast_print(expr);
            format_expr_ast(expr);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
            while('\n' != fgetc(stdin)){}
        }
    }
    mpc_cleanup(13, ReplExpr, Expr, SExpr, QExpr, Atom, Int, Float, Radix, Char, String, Bool, Var, WS);
    return 0;
}
