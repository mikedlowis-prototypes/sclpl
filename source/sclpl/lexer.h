/**
  @file lexer.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef LEXER_H
#define LEXER_H

#include "scanner.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct {
    scanner_t* scanner;
} lexer_t;

typedef enum {
    T_VAR, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE, T_RBRACE, T_LBRACK,
    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END, T_END_FILE
} lex_tok_type_t;

typedef struct {
    lex_tok_type_t type;
    const char* file;
    size_t line;
    size_t column;
    void* value;
} lex_tok_t;

lexer_t* lexer_new(char* p_prompt, FILE* p_input);

lex_tok_t* lex_tok_new(lex_tok_type_t type, void* val);

lex_tok_t* lexer_read(lexer_t* p_lexer);

void lexer_skipline(lexer_t* p_lexer);

char* lexer_dup(const char* p_old);

#endif /* LEXER_H */
