/**
    @file lex.h
    @brief TODO: Describe this file
    $Revision$
    $HeadURL$
*/
#ifndef LEX_H
#define LEX_H

typedef struct
{
    int line;
    int column;
    const char* type;
    char* str;
} tok_t;

typedef enum {
    TOK_ID  = 0,
    TOK_NUM = 1,
    TOK_MAX = 2,
} tok_type_t;

tok_t next_token(void);
void record_position(void);
void identifier(void);
void number(void);
void set_type(tok_type_t type);
void consume(void);
void consume_whitespace(void);
void accept(void);

#endif /* LEX_H */
