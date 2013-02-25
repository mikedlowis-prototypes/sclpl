/**
    @file classes.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include "classes.h"
#include "file.h"

bool whitespace(void)
{
    char ch = file_peek();
    return ((' ' == ch) || ('\t' == ch) || ('\n' == ch) || ('\r' == ch));
}

bool digit(void)
{
    char ch = file_peek();
    return (('0' <= ch) && (ch <= '9'));
}

bool hex_digit(void)
{
    char ch = file_peek();
    return (('0' <= ch) && (ch <= '9')) ||
           (('a' <= ch) && (ch <= 'f')) ||
           (('A' <= ch) && (ch <= 'F'));
}

bool token_end(void)
{
    return (whitespace() || file_eof());
}

