/**
    @file lex.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <string.h>
#include <setjmp.h>
#include "lex.h"
#include "tok.h"
#include "classes.h"
#include "file.h"
#include "buf.h"

/* Prototypes
 *****************************************************************************/
static void accept(void);
static void accept_char(tok_type_t tok);
static void abort(void);
static void reset(void);
static void match_and_consume(char ch);
static bool one_or_more(predicate_t pfn);
static void comment(void);
static void punctuation(void);
static void number(void);
static void hexadecimal(void);
static void floating_point(void);
static void identifier(void);

/* Global Variables
 *****************************************************************************/
jmp_buf Jump_Point;

const char* Types[TOK_MAX] = {
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
static void accept(void)
{
    if (!token_end())
        abort();
    else
        tok_accept();
}

static void accept_char(tok_type_t tok)
{
    tok_set_type( Types[tok] );
    tok_consume();
    tok_accept();
}

static void abort(void)
{
    longjmp(Jump_Point,1);
}

static void reset(void)
{
    while( whitespace() )
        tok_discard();
    tok_reset();
}

static void match_and_consume(char ch)
{
    if (matches(ch))
        tok_consume();
    else
        abort();
}

static bool one_or_more(predicate_t pfn)
{
    if (!pfn()) abort();
    while (pfn()) tok_consume();
}

/* Token Matching Functions
 *****************************************************************************/
void next_token(tok_t* p_token)
{
    reset();
    if (!file_eof())
    {
        /* Mark our starting point so we can resume if we abort */
        if (0 == setjmp(Jump_Point))
        {
            if (matches('#')) comment();

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
        if (0 == strcmp(tok_string(),"end"))
            tok_set_type(Types[TOK_TERM]);

    }
    tok_copy( p_token );
}

static void comment(void)
{
    while (!matches('\n'))
        tok_discard();
    while( whitespace() )
        tok_discard();
}

static void punctuation(void)
{
    switch (file_peek())
    {
        case '(': accept_char( TOK_LPAR );   break;
        case ')': accept_char( TOK_RPAR );   break;
        case '[': accept_char( TOK_LBRACK ); break;
        case ']': accept_char( TOK_RBRACK ); break;
        case '{': accept_char( TOK_LBRACE ); break;
        case '}': accept_char( TOK_RBRACE ); break;
        case ';': accept_char( TOK_TERM );   break;
        default:  identifier(); break;
    }
}

static void number(void)
{
    tok_set_type(Types[TOK_NUM]);
    if (matches('h'))
    {
        hexadecimal();
    }
    else
    {
        floating_point();
        if (matches_any("eE"))
        {
            tok_consume();
            floating_point();
        }
    }
    accept();
}

static void hexadecimal(void)
{
    match_and_consume('h');
    one_or_more( hex_digit );
}

static void floating_point(void)
{
    if (matches('-')) tok_consume();
    one_or_more( digit );
    if (matches('.'))
    {
        tok_consume();
        one_or_more( digit );
    }
}

static void identifier(void)
{
    tok_set_type(Types[TOK_ID]);
    while (!token_end())
        tok_consume();
    accept();
}

