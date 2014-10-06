/**
  @file parser.h
  @brief A collection of helper functions for implementing recursive descent parsers.
  $Revision$
  $HeadURL$
*/
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "vec.h"
#include "exn.h"

DECLARE_EXCEPTION(ParseException);

typedef enum { ATOM, TREE } tree_tag_t;

typedef struct {
    tree_tag_t tag;
    union {
        lex_tok_t* tok;
        vec_t* vec;
    } ptr;
} tree_t;

typedef struct {
    lexer_t* p_lexer;
    lex_tok_t* p_tok;
    vec_t* p_tok_buf;
} parser_t;

parser_t* parser_new(char* p_prompt, FILE* input);

tree_t* parser_tree_new(tree_tag_t tag, void* p_obj);

void parser_fetch(parser_t* p_parser);

lex_tok_t* parser_peek(parser_t* p_parser);

bool parser_eof(parser_t* p_parser);

void parser_resume(parser_t* p_parser);

void parser_error(parser_t* p_parser, const char* p_text);

bool parser_accept(parser_t* p_parser, lex_tok_type_t type);

bool parser_accept_str(parser_t* p_parser, lex_tok_type_t type, const char* p_text);

bool parser_expect(parser_t* p_parser, lex_tok_type_t type);

bool parser_expect_str(parser_t* p_parser, lex_tok_type_t type, const char* p_text);

size_t parser_mark(parser_t* p_parser);

void parser_reduce(parser_t* p_parser, size_t mark);

tree_t* parser_get_tree(parser_t* p_parser);

#endif /* PARSER_H */
