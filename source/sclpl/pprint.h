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

void pprint_token_type(FILE* file, lex_tok_t* token);

void pprint_token_value(FILE* file, lex_tok_t* token);

void pprint_token(FILE* file, lex_tok_t* token, bool print_loc);

void pprint_tree(FILE* file, tree_t* tree, int depth);

#endif /* PPRINT_H */
