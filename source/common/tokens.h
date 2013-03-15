/**
    @file tokens.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef TOKENS_H
#define TOKENS_H

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
} tok_type_t;

#endif /* TOKENS_H */
