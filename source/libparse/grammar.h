/**
  @file grammar.h
  @brief Describes the grammar and parsing rules that form the SCLPL language.
*/
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "parser.h"

tree_t* grammar_toplevel(parser_t* p);
void grammar_require(parser_t* p);
void grammar_type_annotation(parser_t* p);
void grammar_type_definition(parser_t* p);
void grammar_type(parser_t* p);
void grammar_tuple(parser_t* p);
void grammar_function(parser_t* p);
void grammar_definition(parser_t* p);
void grammar_expression(parser_t* p);
void grammar_literal(parser_t* p);
void grammar_arglist(parser_t* p);
void grammar_if_stmnt(parser_t* p);
void grammar_fn_stmnt(parser_t* p);

#endif /* GRAMMAR_H */
