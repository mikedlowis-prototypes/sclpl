/**
  @file grammar.h
  @brief Describes the grammar and parsing rules that form the SCLPL language.
*/
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "parser.h"

AST* grammar_toplevel(Parser* p);
void grammar_require(Parser* p);
void grammar_type_annotation(Parser* p);
void grammar_type_definition(Parser* p);
void grammar_type(Parser* p);
void grammar_tuple(Parser* p);
void grammar_function(Parser* p);
void grammar_definition(Parser* p);
void grammar_expression(Parser* p);
void grammar_literal(Parser* p);
void grammar_arglist(Parser* p);
void grammar_if_stmnt(Parser* p);
void grammar_fn_stmnt(Parser* p);

#endif /* GRAMMAR_H */
