/**
    @file lex.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef LEX_H
#define LEX_H

#include "classes.h"

typedef struct
{
    int line;
    int column;
    const char* type;
    char* str;
} tok_t;

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
    TOK_MAX    = 10,
} tok_type_t;

tok_t next_token(void);
void punctuation(void);
void record_position(void);
void identifier(void);
void number(void);
void hexadecimal(void);
void floating_point(void);
void set_type(tok_type_t type);
void consume(void);
void match_consume(char ch);
void prepare_for_token(void);
void accept_char(tok_type_t type);
void accept(void);
void abort(void);
bool one_or_more(predicate_t pfn);

#endif /* LEX_H */
