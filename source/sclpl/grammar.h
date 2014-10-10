/**
  @file grammar.h
  @brief Describes the grammar and parsing rules that form the SCLPL language.
*/
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "parser.h"

tree_t* grammar_toplevel(parser_t* p_parser);

void grammar_require(parser_t* p_parser);

void grammar_definition(parser_t* p_parser);

void grammar_expression(parser_t* p_parser);

void grammar_literal(parser_t* p_parser);

void grammar_arglist(parser_t* p_parser);

void grammar_if_stmnt(parser_t* p_parser);

void grammar_fn_stmnt(parser_t* p_parser);

#endif /* GRAMMAR_H */
