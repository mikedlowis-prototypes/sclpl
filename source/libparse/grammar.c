/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include <libparse.h>

static void require(Parser* p);
static void type_annotation(Parser* p);
static void type_definition(Parser* p);
static void type(Parser* p);
static void tuple(Parser* p);
static void function(Parser* p);
static void definition(Parser* p);
static void expression(Parser* p);
static void literal(Parser* p);
static void arglist(Parser* p);
static void if_stmnt(Parser* p);
static void fn_stmnt(Parser* p);

AST* toplevel(Parser* p)
{
    AST* p_tree = NULL;
    try {
        if (accept_str(p, T_ID, "require"))
            require(p);
        else if (accept_str(p, T_ID, "type"))
            type_definition(p);
        else if (accept_str(p, T_ID, "ann"))
            type_annotation(p);
        else if (accept_str(p, T_ID, "def"))
            definition(p);
        else
            expression(p);
        //p_tree = get_tree(p);
    } catch(ParseException) {
        /* Do nothing, the tree is bad */
    }
    return p_tree;
}

static void require(Parser* p)
{
    //size_t mrk = mark(p);
    expect(p, T_STRING);
    expect(p, T_END);
    //reduce(p, mrk);
}

static void type_annotation(Parser* p)
{
    //size_t mrk = mark(p);
    expect(p, T_ID);
    type(p);
    expect(p, T_END);
    //reduce(p, mrk);
}

static void type_definition(Parser* p)
{
    //size_t mrk = mark(p);
    expect(p, T_ID);
    expect_str(p, T_ID, "is");
    type(p);
    expect(p, T_END);
    //reduce(p, mrk);
}

static void type(Parser* p) {
    if (accept(p, T_LBRACE)) {
        tuple(p);
    } else {
        expect(p, T_ID);
        if (accept(p, T_LPAR)) {
            function(p);
        }
    }
}

static void tuple(Parser* p) {
    //size_t mrk = mark(p);
    //insert(p, T_ID, lexer_dup("tuple"));
    do {
        type(p);
    } while (accept(p, T_COMMA));
    expect(p, T_RBRACE);
    //reduce(p, mrk);
}

static void function(Parser* p) {
    //size_t mark1 = mark(p) - 1;
    //size_t mark2 = mark(p);
    while (!accept(p, T_RPAR)) {
        type(p);
        if (T_RPAR != peek(p)->type)
            expect(p, T_COMMA);
    }
    //reduce(p, mark2);
    //reduce(p, mark1);
}

static void definition(Parser* p)
{
    //size_t mrk = mark(p);
    expect(p,T_ID);
    if (peek(p)->type == T_LPAR) {
        //insert(p, T_ID, lexer_dup("fn"));
        fn_stmnt(p);
    } else {
        expression(p);
        expect(p,T_END);
    }
    //reduce(p, mrk);
}

static void expression(Parser* p)
{
    if (accept(p, T_LPAR)) {
        //size_t mrk = mark(p);
        expression(p);
        expect(p, T_RPAR);
        //reduce(p, mrk);
    } else if (accept_str(p, T_ID, "if")) {
        if_stmnt(p);
    } else if (accept_str(p, T_ID, "fn")) {
        fn_stmnt(p);
    } else if (peek(p)->type == T_ID) {
        expect(p, T_ID);
        if (peek(p)->type == T_LPAR) {
            arglist(p);
        }
    } else {
        literal(p);
    }
}

static void literal(Parser* p)
{
    switch (peek(p)->type) {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            accept(p, peek(p)->type);
            break;

        default:
            error(p, "Expected a literal");
    }
}

static void arglist(Parser* p)
{
    //size_t mrk = mark(p);
    expect(p, T_LPAR);
    while(peek(p)->type != T_RPAR) {
        expression(p);
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    //reduce(p, mrk);
}

static void if_stmnt(Parser* p)
{
    //size_t mrk = mark(p);
    expression(p);
    expression(p);
    if (accept_str(p, T_ID, "else")) {
        expression(p);
    }
    expect(p,T_END);
    //reduce(p, mrk);
}

static void fn_stmnt(Parser* p)
{
    //size_t mark1 = mark(p);
    expect(p, T_LPAR);
    //size_t mark2 = mark(p);
    while(peek(p)->type != T_RPAR) {
        expect(p, T_ID);
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    //reduce(p, mark2);
    while(peek(p)->type != T_END) {
        expression(p);
    }
    expect(p, T_END);
    //reduce(p, mark1);
}

