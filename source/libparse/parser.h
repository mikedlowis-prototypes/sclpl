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
#include "tree.h"

DECLARE_EXCEPTION(ParseException);

typedef struct {
    Lexer* p_lexer;
    Token* p_tok;
    vec_t* p_tok_buf;
} Parser;

Parser* parser_new(char* p_prompt, FILE* input);

void fetch(Parser* p_parser);

Token* peek(Parser* p_parser);

bool parser_eof(Parser* p_parser);

void parser_resume(Parser* p_parser);

void error(Parser* p_parser, const char* p_text);

bool accept(Parser* p_parser, TokenType type);

bool accept_str(Parser* p_parser, TokenType type, const char* p_text);

bool expect(Parser* p_parser, TokenType type);

bool expect_str(Parser* p_parser, TokenType type, const char* p_text);

size_t mark(Parser* p_parser);

void reduce(Parser* p_parser, size_t mark);

AST* get_tree(Parser* p_parser);

void insert(Parser* p_parser, TokenType type, void* value);

#endif /* PARSER_H */
