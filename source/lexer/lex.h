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
#include "tokens.h"

typedef struct {
    const char* p_text;
    tok_type_t type;
} lex_keyword_t;

void next_token(tok_t* p_token);

#endif /* LEX_H */
