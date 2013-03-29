/**
    @file tokens.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include "tokens.h"

const char* Token_Types[TOK_MAX] = {
    "EOF",    /* TOK_EOF */
    "ID",     /* TOK_ID  */
    "NUM",    /* TOK_NUM */
    "LPAREN", /* TOK_LPAR */
    "RPAREN", /* TOK_RPAR */
    "LBRACK", /* TOK_LBRACK */
    "RBRACK", /* TOK_RBRACK */
    "LBRACE", /* TOK_LBRACE */
    "RBRACE", /* TOK_RBRACE */
    "TERM",   /* TOK_TERM */
    "BOOL",   /* TOK_BOOL */
};

