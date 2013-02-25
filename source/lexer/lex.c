/**
    @file lex.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <string.h>
#include "lex.h"
#include "classes.h"
#include "file.h"
#include "buf.h"

tok_t Token = { 0u };

const char* Token_Strings[TOK_MAX] = {
    "id",  /* TOK_ID  */
    "num", /* TOK_NUM */
};

tok_t next_token(void)
{
    (void)memset(&Token,0,sizeof(Token));
    if (!file_eof())
    {
        consume_whitespace();
        record_position();
        if (digit())
            number();
        //else if (matches('\''))
        //    character();
        //else if (matches('\"'))
        //    string();
        else
            identifier();
    }
    return Token;
}

void number()
{
    set_type(TOK_NUM);

    while (digit()) consume();

    if (!token_end())
        identifier();
    else
        accept();
}

void identifier()
{
    set_type(TOK_ID);
    while (!token_end()) consume();
    accept();
}

void record_position(void)
{
    Token.line   = file_line();
    Token.column = file_column();
}

void set_type(tok_type_t type)
{
    Token.type = Token_Strings[type];
}

void consume(void)
{
    buf_put( file_get() );
}

void consume_whitespace(void)
{
    while( whitespace() )
        (void)file_get();
}

void accept()
{
    Token.str = buf_accept();
}

