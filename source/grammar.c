/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include <sclpl.h>

static AST* require(Parser* p);
static AST* expression(Parser* p);
static AST* literal(Parser* p);
static AST* expect_lit(Parser* p, TokType);
static AST* token_to_tree(Tok* tok);

AST* toplevel(Parser* p)
{
    AST* ret = NULL;
    if (peek(p)->type != T_END_FILE) {
        if (accept_str(p, T_ID, "require"))
            ret = require(p);
        else
            ret = expression(p);
    }
    //printf("%p\n", ret);
    //else if (accept_str(p, T_ID, "type"))
    //    return type_definition(p);
    //else if (accept_str(p, T_ID, "ann"))
    //    return type_annotation(p);
    //else if (accept_str(p, T_ID, "def"))
    //    return definition(p);
    //else
    //    return expression(p);
    return ret;
}

static AST* expression(Parser* p)
{
    if (peek(p)->type == T_ID) {
        return expect_lit(p, T_ID);
        //if (peek(p)->type == T_LPAR) {
        //    arglist(p);
        //}
    } else {
        return literal(p);
    }

    //if (accept(p, T_LPAR)) {
    //    //size_t mrk = mark(p);
    //    expression(p);
    //    expect(p, T_RPAR);
    //    //reduce(p, mrk);
    //} else if (accept_str(p, T_ID, "if")) {
    //    if_stmnt(p);
    //} else if (accept_str(p, T_ID, "fn")) {
    //    fn_stmnt(p);
    //} else if (peek(p)->type == T_ID) {
    //    expect(p, T_ID);
    //    if (peek(p)->type == T_LPAR) {
    //        arglist(p);
    //    }
    //} else {
    //    return literal(p);
    //}
}

static AST* require(Parser* p)
{
    AST* ret = expect_lit(p, T_STRING);
    if (ret != NULL)
        ret = Require(string_value(ret));
    expect(p, T_END);
    return ret;
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
            ret = token_to_tree(tok);
            expect(p, tok->type);
            break;
        default:
            error(p, "Expected a literal");
    }
    return ret;
}

static AST* token_to_tree(Tok* tok)
{
    switch (tok->type) {
        case T_BOOL:   return Bool(tok->value.boolean);
        case T_CHAR:   return Char(tok->value.character);
        case T_STRING: return String(tok->value.text);
        case T_INT:    return Integer(tok->value.integer);
        case T_FLOAT:  return Float(tok->value.floating);
        case T_ID:     return Ident(tok->value.text);
        default:       return NULL;
    }
}

static AST* expect_lit(Parser* p, TokType type)
{
    Tok* tok = peek(p);
    expect(p, type);
    return token_to_tree(tok);
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

static AST* definition(Parser* p)
{
    ////size_t mrk = mark(p);
    //expect(p,T_ID);
    //if (peek(p)->type == T_LPAR) {
    //    //insert(p, T_ID, lexer_dup("fn"));
    //    fn_stmnt(p);
    //} else {
    //    expression(p);
    //    expect(p,T_END);
    //}
    ////reduce(p, mrk);
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

static AST* if_stmnt(Parser* p)
{
    ////size_t mrk = mark(p);
    //expression(p);
    //expression(p);
    //if (accept_str(p, T_ID, "else")) {
    //    expression(p);
    //}
    //expect(p,T_END);
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
