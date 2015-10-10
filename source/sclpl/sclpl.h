/**
  @file sclpl.h
*/
#ifndef SCLPL_H
#define SCLPL_H

#include <opts.h>
#include <libparse.h>
#include <stdio.h>

/* Pretty Printing Data Structures */
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
//void pprint_tree(FILE* file, AST* tree, int depth);

#endif /* SCLPL_H */
