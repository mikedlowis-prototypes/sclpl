/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "grammar.h"
#include "lexer.h"
#include "exn.h"

tree_t* grammar_toplevel(parser_t* p)
{
    tree_t* p_tree = NULL;
    try {
        if (parser_accept_str(p, T_ID, "require"))
            grammar_require(p);
        else if (parser_accept_str(p, T_ID, "type"))
            grammar_type_definition(p);
        else if (parser_accept_str(p, T_ID, "ann"))
            grammar_type_annotation(p);
        else if (parser_accept_str(p, T_ID, "def"))
            grammar_definition(p);
        else
            grammar_expression(p);
        p_tree = parser_get_tree(p);
    } catch(ParseException) {
        fprintf(stderr, "Invalid Syntax\n");
    }
    return p_tree;
}

void grammar_require(parser_t* p)
{
    size_t mark = parser_mark(p);
    parser_expect(p, T_STRING);
    parser_expect(p, T_END);
    parser_reduce(p, mark);
}

void grammar_type_annotation(parser_t* p)
{
    size_t mark = parser_mark(p);
    parser_expect(p, T_ID);
    grammar_type(p);
    parser_expect(p, T_END);
    parser_reduce(p, mark);
}

void grammar_type_definition(parser_t* p)
{
    size_t mark = parser_mark(p);
    parser_expect(p, T_ID);
    parser_expect_str(p, T_ID, "is");
    grammar_type(p);
    parser_expect(p, T_END);
    parser_reduce(p, mark);
}

void grammar_type(parser_t* p) {
    if (parser_accept(p, T_LBRACE)) {
        grammar_tuple(p);
    } else {
        parser_expect(p, T_ID);
        if (parser_accept(p, T_LPAR)) {
            grammar_function(p);
        }
    }
}

void grammar_tuple(parser_t* p) {
    size_t mark = parser_mark(p);
    parser_insert(p, T_ID, lexer_dup("tuple"));
    do {
        grammar_type(p);
    } while (parser_accept(p, T_COMMA));
    parser_expect(p, T_RBRACE);
    parser_reduce(p, mark);
}

void grammar_function(parser_t* p) {
    size_t mark1 = parser_mark(p) - 1;
    size_t mark2 = parser_mark(p);
    while (!parser_accept(p, T_RPAR)) {
        grammar_type(p);
        if (T_RPAR != parser_peek(p)->type)
            parser_expect(p, T_COMMA);
    }
    parser_reduce(p, mark2);
    parser_reduce(p, mark1);
}

void grammar_definition(parser_t* p)
{
    size_t mark = parser_mark(p);
    parser_expect(p,T_ID);
    if (parser_peek(p)->type == T_LPAR) {
        parser_insert(p, T_ID, lexer_dup("fn"));
        grammar_fn_stmnt(p);
    } else {
        grammar_expression(p);
        parser_expect(p,T_END);
    }
    parser_reduce(p, mark);
}

void grammar_expression(parser_t* p)
{
    if (parser_accept(p, T_LPAR)) {
        size_t mark = parser_mark(p);
        grammar_expression(p);
        parser_expect(p, T_RPAR);
        parser_reduce(p, mark);
    } else if (parser_accept_str(p, T_ID, "if")) {
        grammar_if_stmnt(p);
    } else if (parser_accept_str(p, T_ID, "fn")) {
        grammar_fn_stmnt(p);
    } else if (parser_peek(p)->type == T_ID) {
        parser_expect(p, T_ID);
        if (parser_peek(p)->type == T_LPAR) {
            grammar_arglist(p);
        }
    } else {
        grammar_literal(p);
    }
}

void grammar_literal(parser_t* p)
{
    switch (parser_peek(p)->type) {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            parser_accept(p, parser_peek(p)->type);
            break;

        default:
            parser_error(p, "Not a valid expression");
    }
}

void grammar_arglist(parser_t* p)
{
    size_t mark = parser_mark(p);
    parser_expect(p, T_LPAR);
    while(parser_peek(p)->type != T_RPAR) {
        grammar_expression(p);
        if(parser_peek(p)->type != T_RPAR)
            parser_expect(p, T_COMMA);
    }
    parser_expect(p, T_RPAR);
    parser_reduce(p, mark);
}

void grammar_if_stmnt(parser_t* p)
{
    size_t mark = parser_mark(p);
    grammar_expression(p);
    grammar_expression(p);
    if (parser_accept_str(p, T_ID, "else")) {
        grammar_expression(p);
    }
    parser_expect(p,T_END);
    parser_reduce(p, mark);
}

void grammar_fn_stmnt(parser_t* p)
{
    size_t mark1 = parser_mark(p);
    parser_expect(p, T_LPAR);
    size_t mark2 = parser_mark(p);
    while(parser_peek(p)->type != T_RPAR) {
        parser_expect(p, T_ID);
        if(parser_peek(p)->type != T_RPAR)
            parser_expect(p, T_COMMA);
    }
    parser_expect(p, T_RPAR);
    parser_reduce(p, mark2);
    while(parser_peek(p)->type != T_END) {
        grammar_expression(p);
    }
    parser_expect(p, T_END);
    parser_reduce(p, mark1);
}

