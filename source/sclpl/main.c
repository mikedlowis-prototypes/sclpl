#include "mpc.h"
#include "ast.h"
#include <stdio.h>

/* SCLPL Parser
 *****************************************************************************/
/* Grammar is auto generated into 'source/grammar.c' */
extern const char Grammar[];

ast_t* format_expr_ast(mpc_ast_t* expr) {
    ast_t* p_ast = ast_new(UNKNOWN,NULL);
    ast_set_pos(p_ast, "<stdin>", expr->state.row, expr->state.col);

    /* Handle the current node */
    if (0 == strcmp("expr|atom|num|regex", expr->tag)) {
        puts("parsing integer/float");
    } else if (0 == strcmp("num|>", expr->tag)) {
        puts("parsing radix literal");
    } else if (0 == strcmp("str|>", expr->tag)) {
        puts("parsing string literal");
    } else if (0 == strcmp("ch|>", expr->tag)) {
        puts("parsing char literal");
    } else if (0 == strcmp("expr|atom|var|regex", expr->tag)) {
        puts("parsing variable");
    } else if (0 == strcmp("expr|atom|bool|string", expr->tag)) {
        puts("parsing boolean");
    } else if (0 == strcmp("expr|>", expr->tag)) {
        ast_set_type(p_ast, SEXPR);
        /* Handle the current node's children */
        for (int i = 0; i < expr->children_num; i++) {
            mpc_ast_t* child = expr->children[i];
            if ((0 != strncmp(child->tag,"ws",2)) &&
                (0 != strncmp(child->tag,"char",4))) {
                ast_add_child(p_ast, format_expr_ast(expr->children[i]));
            }
        }
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
    mpc_parser_t* Num = mpc_new("num");
    mpc_parser_t* Char = mpc_new("ch");
    mpc_parser_t* String = mpc_new("str");
    mpc_parser_t* Bool = mpc_new("bool");
    mpc_parser_t* Var = mpc_new("var");
    mpc_parser_t* WS = mpc_new("ws");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE, Grammar,
        ReplExpr, Expr, SExpr, QExpr, Atom, Num, Char, String, Bool, Var, WS, NULL);
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
    mpc_cleanup(11, ReplExpr, Expr, SExpr, QExpr, Atom, Num, Char, String, Bool, Var, WS);
    return 0;
}
