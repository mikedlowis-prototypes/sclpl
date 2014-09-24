/**
  @file lexer.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef LEXER_H
#define LEXER_H

#include "scanner.h"
#include "mpc.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct {
    mpc_parser_t* lexrule;
    scanner_t* scanner;
} lexer_t;

typedef enum {
    END, STRING, CHAR, INT, FLOAT, BOOL, LBRACE, RBRACE, LBRACK, RBRACK, LPAR,
    RPAR, COMMA, VAR, END_FILE
} lex_tok_type_t;

typedef struct {
    lex_tok_type_t type;
    const char* file;
    size_t line;
    size_t column;
    void* value;
} lex_tok_t;

lexer_t* lexer_new(char* p_prompt, FILE* p_input);

lex_tok_t* lexer_read(lexer_t* p_lexer);

#endif /* LEXER_H */
