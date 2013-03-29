/**
    @file tok.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef TOK_H
#define TOK_H

#include <stdbool.h>
#include <stdint.h>
#include "tokens.h"

typedef struct
{
    tok_type_t type;
    char* p_text;
    char* p_file_name;
    long line;
    long column;
} tok_t;

bool tok_source(char* fname);
bool tok_eof(void);
void tok_finish(void);
tok_t* tok_read(void);
void tok_read_fname(void);
tok_t* tok_build_token(void);
bool tok_next_field(uint32_t* end);
void tok_read_field(uint32_t index, char* str);
void tok_fatal_error(uint32_t err_code);

#endif /* TOK_H */
