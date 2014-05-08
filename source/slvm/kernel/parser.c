/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "parser.h"
#include "pal.h"
#include <stdbool.h>
#include <string.h>
//#include <stdlib.h>

long strtol(char* p_str, char** p_end);

/* Fetching Tokens
 *****************************************************************************/
static void skip_whitespace(void);
static void skip_comment(void);
static char* read_string(void);
static char* read_token(void);
static char* grow_token(size_t* p_size, size_t* p_index, char* p_str, char ch);
static bool is_whitespace(void);

char* fetch_token(void)
{
    char* result = NULL;
    skip_whitespace();
    switch(pal_peek_char())
    {
        case '#':
            skip_comment();
            result = fetch_token();
            break;

        case '"':
            result = read_string();
            break;

        case EOF:
            break;

        default:
            result = read_token();
            break;

    }
    return result;
}

static char* read_string(void)
{
    size_t strsize  = 8;
    size_t strindex = 0;
    char*  string   = (char*)pal_allocate(strsize);

    string = grow_token(&strsize, &strindex, string, pal_read_char());
    while('"' != pal_peek_char())
    {
        if(pal_is_eof()) {
            pal_free(string);
            return NULL;
        }
        string = grow_token(&strsize, &strindex, string, pal_read_char());
    }
    string = grow_token(&strsize, &strindex, string, pal_read_char());

    return string;
}

static char* read_token(void)
{
    size_t strsize  = 8;
    size_t strindex = 0;
    char*  string   = (char*)pal_allocate(strsize);

    while(!is_whitespace())
    {
        if(pal_is_eof()) {
            pal_free(string);
            return NULL;
        }
        string = grow_token(&strsize, &strindex, string, pal_read_char());
    }

    return string;
}

static char* grow_token(size_t* p_size, size_t* p_index, char* p_str, char ch)
{
    /* If we're about to be too big for the string */
    if( (*(p_index) + 1) >= *(p_size) )
    {
        /* Double the string size */
        *(p_size) *= 2;
        p_str = (char*)pal_reallocate( p_str, *(p_size) );
    }
    /* Set the current char to the provided value and null terminate the str */
    p_str[ *(p_index) ]  = ch;
    p_str[ *(p_index)+1] = '\0';
    *(p_index) += 1;
    /* Return the (possibly moved) string */
    return p_str;
}

static void skip_whitespace(void)
{
    while(!pal_is_eof() && is_whitespace())
    {
        (void)pal_read_char();
    }
}

static void skip_comment(void)
{
    while(!pal_is_eof() && ('\n' != pal_read_char()))
    {
    }
}

static bool is_whitespace(void)
{
    char ch = pal_peek_char();
    return ((ch == ' ')  || (ch == '\t') || (ch == '\r') || (ch == '\n'));
}

/* Parsing Tokens
 *****************************************************************************/
static bool is_integer(char* p_str, val_t* p_val);
static bool is_float(char* p_str, val_t* p_val);
static bool is_string(char* p_str, val_t* p_val);
static bool is_char(char* p_str, val_t* p_val);

TokenType_T parse_token(char* str, val_t* p_val)
{
    TokenType_T type = ERROR;
    if(str != NULL)
    {
        if(is_integer(str,p_val))
        {
            type = INTEGER;
        }
        else if(is_float(str,p_val))
        {
            type = FLOAT;
        }
        else if(is_string(str,p_val))
        {
            type = STRING;
        }
        else if(is_char(str,p_val))
        {
            type = CHAR;
        }
        else
        {
            type = WORD;
            *(p_val) = (val_t)str;
        }
    }
    return type;
}

static bool is_integer(char* p_str, val_t* p_val)
{
    char* end;
    *(p_val) = (val_t)strtol(p_str,&end);
    return (end == &(p_str[pal_strlen(p_str)]));
    //return false;
}

static bool is_float(char* p_str, val_t* p_val)
{
    return false;
}

static bool is_string(char* p_str, val_t* p_val)
{
    bool res = false;
    if((p_str[0] == '"') && (p_str[pal_strlen(p_str)-1] == '"'))
    {
        /* Cut off the last double quote */
        p_str[pal_strlen(p_str)-1] = '\0';
        /* And return the string after the first double quote */
        *(p_val) = (val_t)(p_str+1);
        res = true;
    }
    return res;
}

static bool is_char(char* p_str, val_t* p_val)
{
    static char const * const named_chars[] = {
        "\a\0alarm",
        "\b\0backspace",
        "\f\0formfeed",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab",
        "\0\0null",
        " \0space",
        NULL
    };

    bool res = false;
    /* If the string starts with a char indicator (backslash) */
    if (p_str[0] == '\\')
    {
        size_t length = pal_strlen(p_str);
        /* and it only has one character following it */
        if(length == 2)
        {
            /* Return the character and a status of true */
            *(p_val) = p_str[1];
            res = true;
        }
        /* else, if the length is greater than two, try to look it up */
        else if(length > 2)
        {
            size_t index = 0;
            /* Loop through all named chars till we find a match or reach the
             * end */
            while(named_chars[index])
            {
                /* If we found a match */
                if( 0 == pal_strcmp( (p_str + 1), (named_chars[index] + 2) ) )
                {
                    /* Return the character value and indicate success */
                    *(p_val) = named_chars[index][0];
                    res = true;
                }
                index++;
            }
        }
    }
    return res;
}

/* Parsing Numbers
 *****************************************************************************/
static bool is_ws(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

long strtol(char* p_str, char** p_p_end)
{
    int ch;
    int sign = 1;
    int base = 10;
    long value = 0;

    /* Skip any leading whitespace */
    do {
        ch = *(p_str);
        p_str++;
    } while (is_ws(ch));

    /* Detect leading sign */
    if (ch == '-')
    {
        sign = -1;
        ch = *p_str;
        p_str++;
    }
    else if (ch == '+')
    {
        ch = *p_str;
        p_str++;
    }

    /* Detect the base of the number being parsed */
    if((ch == '0') && (*p_str == 'x' || *p_str == 'X'))
    {
        base = 16;
        ch = p_str[1];
        p_str += 2;
    }
    else if((ch == '0') && (*p_str == 'b' || *p_str == 'B'))
    {
        base = 2;
        ch = p_str[1];
        p_str += 2;
    }

    /* Accumulate the digits in the appropriate base */
    for (value = 0;; ch = *p_str++)
    {
        /* Determine the value of the current digit */
        if (ch >= '0' && ch <= '9')
            ch -= '0';
        else if ((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
            ch -= (ch >= 'A' && ch <= 'F') ? 'A'-10 : 'a'-10;
        else
            break;

        /* if the base digit is invalid for the selected base then quit */
        if (ch >= base) break;

        /* Shift the value be the base and add in the digit */
        value *= base;
        value += ch;
    }

    *p_p_end = p_str-1;
    return sign * value;
}
