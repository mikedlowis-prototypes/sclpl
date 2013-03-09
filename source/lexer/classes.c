/**
    @file classes.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <string.h>
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
           (('A' <= ch) && (ch <= 'F'));
}

bool token_end(void)
{
    return (whitespace() || matches_any("()[]{};") || file_eof());
}

bool matches(char ch)
{
    return (ch == file_peek());
}

bool matches_any(char* str)
{
    bool ret = false;
    char ch = file_peek();
    int len = strlen(str);
    int i;
    for (i=0; i < len; i++)
    {
        if (ch == str[i])
        {
            ret = true;
            break;
        }
    }
    return ret;
}

