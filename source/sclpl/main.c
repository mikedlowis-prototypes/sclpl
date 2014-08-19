#include "mpc.h"
#include <stdio.h>

/* SCLPL Parser
 *****************************************************************************/
/* Grammar is auto generated into 'source/grammar.c' */
extern const char Grammar[];

mpc_ast_t* format_expr_ast(mpc_ast_t* expr) {
    return expr;
}

int main(int argc, char **argv) {
    mpc_parser_t* ReplExpr = mpc_new("replexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Atom = mpc_new("atom");
    mpc_parser_t* Num = mpc_new("num");
    mpc_parser_t* Char = mpc_new("ch");
    mpc_parser_t* String = mpc_new("str");
    mpc_parser_t* Bool = mpc_new("bool");
    mpc_parser_t* Var = mpc_new("var");
    mpc_parser_t* WS = mpc_new("ws");
    mpca_lang(MPCA_LANG_WHITESPACE_SENSITIVE, Grammar,
        ReplExpr, Expr, Atom, Num, Char, String, Bool, Var, WS, NULL);
    while(!feof(stdin)) {
        mpc_result_t r;
        printf(":> ");
        if (mpc_parse_pipe("<stdin>", stdin, ReplExpr, &r)) {
            mpc_ast_t* expr = (mpc_ast_t*)(((mpc_ast_t*)r.output)->children[1]);
            mpc_ast_print(format_expr_ast(expr));
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
            while('\n' != fgetc(stdin)){}
        }
    }
    mpc_cleanup(9, ReplExpr, Expr, Atom, Num, Char, String, Bool, Var, WS);
    return 0;
}
