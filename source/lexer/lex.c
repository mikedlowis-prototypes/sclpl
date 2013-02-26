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
};

tok_t next_token(void)
{
    prepare_for_token();
    if (!file_eof())
    {
        if (matches_any("()[]{};"))
            punctuation();
        else if (digit())
            number();
        //else if (matches('\''))
        //    character();
        //else if (matches('\"'))
        //    string();
        else
            identifier();

        /* the keyword "end" is actually a TOK_TERM */
        if (0 == strcmp(Token.str,"end"))
            set_type(TOK_TERM);
    }
    return Token;
}

void punctuation(void)
{
    switch (file_peek())
    {
        case '(': accept_char( TOK_LPAR ); break;
        case ')': accept_char( TOK_RPAR ); break;
        case '[': accept_char( TOK_LBRACK ); break;
        case ']': accept_char( TOK_RBRACK ); break;
        case '{': accept_char( TOK_LBRACE ); break;
        case '}': accept_char( TOK_RBRACE ); break;
        case ';': accept_char( TOK_TERM ); break;
        default:  identifier(); break;
    }
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
    while (!token_end() && !matches_any("()[]{};")) consume();
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

void prepare_for_token(void)
{
    (void)memset(&Token,0,sizeof(Token));
    while( whitespace() )
        (void)file_get();
    record_position();
}

void accept_char(tok_type_t type)
{
    set_type(type);
    consume();
    accept();
}

void accept()
{
    Token.str = buf_accept();
}

