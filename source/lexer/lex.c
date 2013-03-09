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
static void keyword(void);
static void comment(void);
static void punctuation(void);
static void number(void);
static void hexadecimal(void);
static void floating_point(void);
static void exponent(void);
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
    "BOOL",   /* TOK_BOOL */
};

const lex_keyword_t Keywords[] = {
    { "end",   TOK_TERM },
    { "true",  TOK_BOOL },
    { "false", TOK_BOOL },
    { NULL,    TOK_MAX}
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
            else if (matches('-') || digit())
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
        keyword();
    }
    tok_copy( p_token );
}

static void keyword(void)
{
    const char* p_text = tok_string();
    int i = 0;
    while ( Keywords[i].p_text != NULL)
    {
        if (0 == strcmp( p_text, Keywords[i].p_text ))
        {
            tok_set_type( Types[ Keywords[i].type ] );
            break;
        }
        i++;
    }
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
    if (matches('0'))
    {
        tok_consume();
        if (matches('x'))
        {
            tok_consume();
            hexadecimal();
        }
        else if (matches('-'))
            abort();
        else if (!token_end())
        {
            floating_point();
            if (!token_end()) exponent();
        }
    }
    else
    {
        floating_point();
        if (!token_end()) exponent();
    }
    accept();
}

static void hexadecimal(void)
{
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

static void exponent(void)
{
    match_and_consume('e');
    floating_point();
}

static void identifier(void)
{
    tok_set_type(Types[TOK_ID]);
    while (!token_end())
        tok_consume();
    accept();
}

