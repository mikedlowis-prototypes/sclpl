/**
  @file pprint.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef PPRINT_H
#define PPRINT_H

#include "parser.h"
#include "lexer.h"

void pprint_token_type(FILE* file, Token* token);

void pprint_token_value(FILE* file, Token* token);

void pprint_token(FILE* file, Token* token, bool print_loc);

void pprint_tree(FILE* file, AST* tree, int depth);

#endif /* PPRINT_H */
