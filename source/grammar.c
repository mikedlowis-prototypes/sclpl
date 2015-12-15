/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include <sclpl.h>

static AST* require(Parser* p);
static AST* definition(Parser* p);
static AST* expression(Parser* p);
static AST* if_stmnt(Parser* p);
static AST* function(Parser* p);
static AST* literal(Parser* p);
static AST* expr_block(Parser* p);
static AST* token_to_tree(Tok* tok);
static AST* func_app(Parser* p, AST* fn);

AST* toplevel(Parser* p)
{
    AST* ret = NULL;
    if (!match(p, T_END_FILE)) {
        if (accept_str(p, T_ID, "require"))
            ret = require(p);
        else if (accept_str(p, T_ID, "def"))
            ret = definition(p);
        else
            ret = expression(p);
    }
    //printf("%p\n", ret);
    //else if (accept_str(p, T_ID, "type"))
    //    return type_definition(p);
    //else if (accept_str(p, T_ID, "ann"))
    //    return type_annotation(p);
    return ret;
}

static AST* definition(Parser* p)
{
    Tok* id = expect(p, T_ID);
    AST* expr;
    if (peek(p)->type == T_LPAR) {
        expr = function(p);
    } else {
        expr = expression(p);
        expect(p, T_END);
    }
    return Def(id, expr);
}

static AST* require(Parser* p)
{
    AST* ast = Require(expect(p, T_STRING));
    expect(p, T_END);
    return ast;
}

static AST* expression(Parser* p)
{
    AST* expr = NULL;
    if (accept(p, T_LPAR)) {
        expr = expression(p);
        expect(p, T_RPAR);
    } else if (accept_str(p, T_ID, "if")) {
        expr = if_stmnt(p);
    } else if (accept_str(p, T_ID, "fn")) {
        expr = function(p);
    } else if (match(p, T_ID)) {
        expr = Ident(expect(p,T_ID));
    } else {
        expr = literal(p);
    }
    /* Check if this is a function application */
    if (peek(p)->type == T_LPAR) {
        expr = func_app(p, expr);
    }
    return expr;
}

static AST* if_stmnt(Parser* p)
{
    AST* ifexpr = IfExpr();
    ifexpr_set_cond( ifexpr, expression(p) );
    accept_str(p, T_ID, "then");
    ifexpr_set_then( ifexpr, expr_block(p) );
    if (accept_str(p, T_ID, "else"))
        ifexpr_set_else( ifexpr, expr_block(p) );
    expect(p,T_END);
    return ifexpr;
}

static AST* function(Parser* p)
{
    AST* func = Func();
    expect(p, T_LPAR);
    while(peek(p)->type != T_RPAR) {
        func_add_arg(func, Ident(expect(p,T_ID)));
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    func_set_body(func, expr_block(p));
    expect(p, T_END);
    return func;
}

static AST* literal(Parser* p)
{
    AST* ret = NULL;
    Tok* tok = peek(p);
    switch (tok->type) {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            ret = token_to_tree(expect(p, tok->type));
            break;
        default:
            error(p, "Expected a literal");
    }
    return ret;
}

static AST* expr_block(Parser* p)
{
    AST* block = Block();
    do {
        block_append(block, expression(p));
    } while(!match(p, T_END) && !match_str(p, T_ID, "else"));
    return block;
}

static AST* token_to_tree(Tok* tok)
{
    switch (tok->type) {
        case T_BOOL:   return Bool(tok);
        case T_CHAR:   return Char(tok);
        case T_STRING: return String(tok);
        case T_INT:    return Integer(tok);
        case T_FLOAT:  return Float(tok);
        case T_ID:     return Ident(tok);
        default:       return NULL;
    }
}

static AST* func_app(Parser* p, AST* fn)
{
    AST* app = FnApp(fn);
    expect(p,T_LPAR);
    while (peek(p)->type != T_RPAR) {
        fnapp_add_arg(p, expression(p));
        if (peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p,T_RPAR);
    return app;
}


#if 0
static AST* type_annotation(Parser* p)
{
    //shifttok(p, T_ID);
    //type(p);
    //expect(p, T_END);
    //reduce(Annotation);
    return NULL;
}

static AST* type_definition(Parser* p)
{
    //expect(p, T_ID);
    //expect_str(p, T_ID, "is");
    //type(p);
    //expect(p, T_END);
    return NULL;
}

static AST* type(Parser* p) {
    //if (accept(p, T_LBRACE)) {
    //    tuple(p);
    //} else {
    //    expect(p, T_ID);
    //    if (accept(p, T_LPAR)) {
    //        function(p);
    //    }
    //}
    return NULL;
}

static AST* tuple(Parser* p) {
    ////size_t mrk = mark(p);
    ////insert(p, T_ID, lexer_dup("tuple"));
    //do {
    //    type(p);
    //} while (accept(p, T_COMMA));
    //expect(p, T_RBRACE);
    ////reduce(p, mrk);
    return NULL;
}
#endif
