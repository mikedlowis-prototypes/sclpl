/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include <libparse.h>

AST* grammar_toplevel(Parser* p)
{
    AST* p_tree = NULL;
    try {
        if (accept_str(p, T_ID, "require"))
            grammar_require(p);
        else if (accept_str(p, T_ID, "type"))
            grammar_type_definition(p);
        else if (accept_str(p, T_ID, "ann"))
            grammar_type_annotation(p);
        else if (accept_str(p, T_ID, "def"))
            grammar_definition(p);
        else
            grammar_expression(p);
        p_tree = get_tree(p);
    } catch(ParseException) {
        /* Do nothing, the tree is bad */
    }
    return p_tree;
}

void grammar_require(Parser* p)
{
    size_t mrk = mark(p);
    expect(p, T_STRING);
    expect(p, T_END);
    reduce(p, mrk);
}

void grammar_type_annotation(Parser* p)
{
    size_t mrk = mark(p);
    expect(p, T_ID);
    grammar_type(p);
    expect(p, T_END);
    reduce(p, mrk);
}

void grammar_type_definition(Parser* p)
{
    size_t mrk = mark(p);
    expect(p, T_ID);
    expect_str(p, T_ID, "is");
    grammar_type(p);
    expect(p, T_END);
    reduce(p, mrk);
}

void grammar_type(Parser* p) {
    if (accept(p, T_LBRACE)) {
        grammar_tuple(p);
    } else {
        expect(p, T_ID);
        if (accept(p, T_LPAR)) {
            grammar_function(p);
        }
    }
}

void grammar_tuple(Parser* p) {
    size_t mrk = mark(p);
    insert(p, T_ID, lexer_dup("tuple"));
    do {
        grammar_type(p);
    } while (accept(p, T_COMMA));
    expect(p, T_RBRACE);
    reduce(p, mrk);
}

void grammar_function(Parser* p) {
    size_t mark1 = mark(p) - 1;
    size_t mark2 = mark(p);
    while (!accept(p, T_RPAR)) {
        grammar_type(p);
        if (T_RPAR != peek(p)->type)
            expect(p, T_COMMA);
    }
    reduce(p, mark2);
    reduce(p, mark1);
}

void grammar_definition(Parser* p)
{
    size_t mrk = mark(p);
    expect(p,T_ID);
    if (peek(p)->type == T_LPAR) {
        insert(p, T_ID, lexer_dup("fn"));
        grammar_fn_stmnt(p);
    } else {
        grammar_expression(p);
        expect(p,T_END);
    }
    reduce(p, mrk);
}

void grammar_expression(Parser* p)
{
    if (accept(p, T_LPAR)) {
        size_t mrk = mark(p);
        grammar_expression(p);
        expect(p, T_RPAR);
        reduce(p, mrk);
    } else if (accept_str(p, T_ID, "if")) {
        grammar_if_stmnt(p);
    } else if (accept_str(p, T_ID, "fn")) {
        grammar_fn_stmnt(p);
    } else if (peek(p)->type == T_ID) {
        expect(p, T_ID);
        if (peek(p)->type == T_LPAR) {
            grammar_arglist(p);
        }
    } else {
        grammar_literal(p);
    }
}

void grammar_literal(Parser* p)
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

void grammar_arglist(Parser* p)
{
    size_t mrk = mark(p);
    expect(p, T_LPAR);
    while(peek(p)->type != T_RPAR) {
        grammar_expression(p);
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    reduce(p, mrk);
}

void grammar_if_stmnt(Parser* p)
{
    size_t mrk = mark(p);
    grammar_expression(p);
    grammar_expression(p);
    if (accept_str(p, T_ID, "else")) {
        grammar_expression(p);
    }
    expect(p,T_END);
    reduce(p, mrk);
}

void grammar_fn_stmnt(Parser* p)
{
    size_t mark1 = mark(p);
    expect(p, T_LPAR);
    size_t mark2 = mark(p);
    while(peek(p)->type != T_RPAR) {
        expect(p, T_ID);
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    reduce(p, mark2);
    while(peek(p)->type != T_END) {
        grammar_expression(p);
    }
    expect(p, T_END);
    reduce(p, mark1);
}

