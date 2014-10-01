/**
  @file grammar.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "grammar.h"

tree_t* grammar_toplevel(parser_t* p_parser)
{
    if (parser_accept_str(p_parser, T_VAR, "import"))
        grammar_import(p_parser);
    else if (parser_accept_str(p_parser, T_VAR, "def"))
        grammar_definition(p_parser);
    else if (p_parser->p_lexer->scanner->p_input == stdin)
        grammar_expression(p_parser);
    else
        parser_error(p_parser, "Unrecognized top-level form");
    return parser_get_tree(p_parser);
}

void grammar_import(parser_t* p_parser)
{
    size_t mark = parser_mark(p_parser);
    parser_expect(p_parser, T_VAR);
    parser_expect(p_parser, T_END);
    parser_reduce(p_parser, mark);
}

void grammar_definition(parser_t* p_parser)
{
    size_t mark = parser_mark(p_parser);
    parser_expect(p_parser,T_VAR);
    if (parser_peek(p_parser)->type == T_LPAR) {
        grammar_fn_stmnt(p_parser);
    } else {
        grammar_expression(p_parser);
        parser_expect(p_parser,T_END);
    }
    parser_reduce(p_parser, mark);
}

void grammar_expression(parser_t* p_parser)
{
    if (parser_accept(p_parser, T_LPAR)) {
        size_t mark = parser_mark(p_parser);
        grammar_expression(p_parser);
        parser_expect(p_parser, T_RPAR);
        parser_reduce(p_parser, mark);
    } else if (parser_accept_str(p_parser, T_VAR, "if")) {
        grammar_if_stmnt(p_parser);
    } else if (parser_accept_str(p_parser, T_VAR, "fn")) {
        grammar_fn_stmnt(p_parser);
    } else if (parser_peek(p_parser)->type == T_VAR) {
        parser_expect(p_parser, T_VAR);
        if (parser_peek(p_parser)->type == T_LPAR) {
            grammar_arglist(p_parser);
        }
    } else {
        grammar_literal(p_parser);
    }
}

void grammar_literal(parser_t* p_parser)
{
    switch (parser_peek(p_parser)->type)
    {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            parser_accept(p_parser, parser_peek(p_parser)->type);
            break;

        default:
            parser_error(p_parser, "Not a valid expression");
            break;
    }
}

void grammar_arglist(parser_t* p_parser)
{
    size_t mark = parser_mark(p_parser);
    parser_expect(p_parser, T_LPAR);
    while(parser_peek(p_parser)->type != T_RPAR) {
        grammar_expression(p_parser);
        if(parser_peek(p_parser)->type != T_RPAR)
            parser_expect(p_parser, T_COMMA);
    }
    parser_expect(p_parser, T_RPAR);
    parser_reduce(p_parser, mark);
}

void grammar_if_stmnt(parser_t* p_parser)
{
    size_t mark = parser_mark(p_parser);
    grammar_expression(p_parser);
    grammar_expression(p_parser);
    parser_expect_str(p_parser,T_VAR,"else");
    grammar_expression(p_parser);
    parser_expect(p_parser,T_END);
    parser_reduce(p_parser, mark);
}

void grammar_fn_stmnt(parser_t* p_parser)
{
    size_t mark1 = parser_mark(p_parser);
    parser_expect(p_parser, T_LPAR);
    size_t mark2 = parser_mark(p_parser);
    while(parser_peek(p_parser)->type != T_RPAR) {
        parser_expect(p_parser, T_VAR);
        if(parser_peek(p_parser)->type != T_RPAR)
            parser_expect(p_parser, T_COMMA);
    }
    parser_expect(p_parser, T_RPAR);
    parser_reduce(p_parser, mark2);
    while(parser_peek(p_parser)->type != T_END) {
        grammar_expression(p_parser);
    }
    parser_expect(p_parser, T_END);
    parser_reduce(p_parser, mark1);
}
