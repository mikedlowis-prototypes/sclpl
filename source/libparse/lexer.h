/**
  @file lexer.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct {
    char* p_line;
    size_t index;
    size_t lineno;
    FILE* p_input;
    char* p_prompt;
} Lexer;

typedef enum {
    T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE, T_RBRACE, T_LBRACK,
    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END, T_END_FILE
} TokenType;

typedef struct {
    TokenType type;
    const char* file;
    size_t line;
    size_t col;
    void* value;
} Token;

Lexer* lexer_new(char* p_prompt, FILE* p_input);

Token* lex_tok_new(TokenType type, void* val);

Token* lexer_read(Lexer* p_lexer);

void lexer_skipline(Lexer* p_lexer);

char* lexer_dup(const char* p_old);

#endif /* LEXER_H */
