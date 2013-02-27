/**
    @file tok.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <string.h>
#include "tok.h"
#include "buf.h"

tok_t Token;

void tok_reset(void)
{
    (void)memset(&Token,0,sizeof(Token));
    Token.line   = file_line();
    Token.column = file_column();
}

void tok_copy(tok_t* p_token)
{
    *(p_token) = Token;
}

void tok_set_type(const char* p_str)
{
    Token.type = p_str;
}

char* tok_string(void)
{
    return Token.str;
}

void tok_consume(void)
{
    buf_put( file_get() );
}

void tok_discard(void)
{
    (void)file_get();
}

void tok_accept(void)
{
    Token.str = buf_accept();
}

