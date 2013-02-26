/**
    @file lex.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <string.h>
#include <setjmp.h>
#include "lex.h"
#include "classes.h"
#include "file.h"
#include "buf.h"

/* Global Variables
 *****************************************************************************/
tok_t Token;
jmp_buf Jump_Point;
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

/* Control Functions
 *****************************************************************************/
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

void match_consume(char ch)
{
    if (matches(ch))
        consume();
    else
        abort();
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

void accept(void)
{
    if (!token_end())
        abort();
    else
        Token.str = buf_accept();
}

void abort(void)
{
    longjmp(Jump_Point,1);
}

bool one_or_more(predicate_t pfn)
{
    if (!pfn()) abort();
    while (pfn()) consume();
}

/* Token Matching Functions
 *****************************************************************************/
tok_t next_token(void)
{
    prepare_for_token();
    if (!file_eof())
    {
        /* Mark our starting point so we can resume if we abort */
        if (0 == setjmp(Jump_Point))
        {
            if (matches_any("()[]{};"))
                punctuation();
            else if (matches('-') || digit() || matches('h'))
                number();
            //else if (matches('\''))
            //    character();
            //else if (matches('\"'))
            //    string();
            else
                identifier();
        }
        else
        {
            identifier();
        }

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

void number(void)
{
    set_type(TOK_NUM);
    if (matches('h'))
    {
        hexadecimal();
    }
    else
    {
        floating_point();
        if (matches_any("eE"))
        {
            consume();
            floating_point();
        }
    }
    accept();
}

void hexadecimal(void)
{
    match_consume('h');
    one_or_more( hex_digit );
}

void floating_point(void)
{
    if (matches('-')) consume();
    one_or_more( digit );
    if (matches('.'))
    {
        consume();
        one_or_more( digit );
    }
}

void identifier(void)
{
    set_type(TOK_ID);
    while (!token_end())
        consume();
    accept();
}

