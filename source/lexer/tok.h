/**
    @file tok.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef TOK_H
#define TOK_H

typedef struct
{
    int line;
    int column;
    const char* type;
    char* str;
} tok_t;

void tok_reset(void);
void tok_copy(tok_t* p_token);
void tok_set_type(const char* p_str);
char* tok_string(void);
void tok_consume(void);
void tok_discard(void);
void tok_accept(void);

#endif /* TOK_H */
