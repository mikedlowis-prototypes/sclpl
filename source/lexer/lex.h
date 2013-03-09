/**
    @file lex.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef LEX_H
#define LEX_H

#include "tok.h"
#include "classes.h"

typedef enum {
    TOK_EOF    = 0,
    TOK_ID     = 1,
    TOK_NUM    = 2,
    TOK_LPAR   = 3,
    TOK_RPAR   = 4,
    TOK_LBRACK = 5,
    TOK_RBRACK = 6,
    TOK_LBRACE = 7,
    TOK_RBRACE = 8,
    TOK_TERM   = 9,
    TOK_BOOL   = 10,
    TOK_MAX    = 11,
} lex_tok_t;

typedef struct {
    const char* p_text;
    tok_type_t type;
} lex_keyword_t;

void next_token(tok_t* p_token);

#endif /* LEX_H */
