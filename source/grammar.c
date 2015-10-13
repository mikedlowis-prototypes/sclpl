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
static AST* literal(Parser* p);
static AST* expr_block(Parser* p);
static AST* token_to_tree(Tok* tok);

AST* toplevel(Parser* p)
{
    AST* ret = NULL;
    if (peek(p)->type != T_END_FILE) {
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
    Tok* id = expect(p, T_ID);//expect_lit(p, T_ID);
    AST* expr;
    //if (peek(p)->type == T_LPAR)
    //    expr = function(p);
    //else
        expr = expression(p);
    expect(p, T_END);
    return Def(id, expr);
}

static AST* require(Parser* p)
{
    Tok* tok = expect(p, T_STRING);
    AST* ast = Require(tok);
    expect(p, T_END);
    return ast;
}

static AST* expression(Parser* p)
{
    if (peek(p)->type == T_ID) {
        return Ident(expect(p,T_ID));
        //if (peek(p)->type == T_LPAR) {
        //    arglist(p);
        //}
    } else if (accept_str(p, T_ID, "if")) {
        return if_stmnt(p);
    } else {
        return literal(p);
    }

    //if (accept(p, T_LPAR)) {
    //    //size_t mrk = mark(p);
    //    expression(p);
    //    expect(p, T_RPAR);
    //    //reduce(p, mrk);
    //} else if (accept_str(p, T_ID, "fn")) {
    //    fn_stmnt(p);
    //} else if (peek(p)->type == T_ID) {
    //    expect(p, T_ID);
    //    if (peek(p)->type == T_LPAR) {
    //        arglist(p);
    //    }
}

static AST* if_stmnt(Parser* p)
{
    //AST* ifexpr = IfExpr();
    //ifexpr_set_condition( expression(p) );
    //ifexpr_set_branch_then( expr_block(p) );
    //expect(p,T_END);
    //return ifexpr;
    return NULL;
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
    } while(!accept_str(p, T_ID, "else") && !accept(p, T_END));
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




#if 0

static AST* require(Parser* p);
static AST* type_annotation(Parser* p);
static AST* type_definition(Parser* p);
static AST* type(Parser* p);
static AST* tuple(Parser* p);
static AST* function(Parser* p);
static AST* definition(Parser* p);
static AST* expression(Parser* p);
static AST* arglist(Parser* p);
static AST* if_stmnt(Parser* p);
static AST* fn_stmnt(Parser* p);


static AST* type_annotation(Parser* p)
{
    //shifttok(p, T_ID);
    //type(p);
    //expect(p, T_END);
    //reduce(Annotation);
    return NULL;
}

/*****************************************************************************/

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

static AST* function(Parser* p) {
    ////size_t mark1 = mark(p) - 1;
    ////size_t mark2 = mark(p);
    //while (!accept(p, T_RPAR)) {
    //    type(p);
    //    if (T_RPAR != peek(p)->type)
    //        expect(p, T_COMMA);
    //}
    ////reduce(p, mark2);
    ////reduce(p, mark1);
    return NULL;
}

static AST* arglist(Parser* p)
{
    ////size_t mrk = mark(p);
    //expect(p, T_LPAR);
    //while(peek(p)->type != T_RPAR) {
    //    expression(p);
    //    if(peek(p)->type != T_RPAR)
    //        expect(p, T_COMMA);
    //}
    //expect(p, T_RPAR);
    ////reduce(p, mrk);
    return NULL;
}


static AST* fn_stmnt(Parser* p)
{
    ////size_t mark1 = mark(p);
    //expect(p, T_LPAR);
    ////size_t mark2 = mark(p);
    //while(peek(p)->type != T_RPAR) {
    //    expect(p, T_ID);
    //    if(peek(p)->type != T_RPAR)
    //        expect(p, T_COMMA);
    //}
    //expect(p, T_RPAR);
    ////reduce(p, mark2);
    //while(peek(p)->type != T_END) {
    //    expression(p);
    //}
    //expect(p, T_END);
    ////reduce(p, mark1);
    return NULL;
}

#endif
