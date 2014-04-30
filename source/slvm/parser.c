/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "parser.h"

/* Fetching Tokens
 *****************************************************************************/
static void skip_whitespace(FILE* input);
static void skip_comment(FILE* input);
static void parse_string(FILE* input);
static void parse_token(FILE* input);
static void grow_token(size_t* p_size, size_t* p_index, char** p_p_str, char ch);
static bool is_whitespace(FILE* input);
static char fpeekc(FILE* input);

char* fetch(FILE* input)
{
    char* result = NULL;
    skip_whitespace();
    switch(fpeekc(input))
    {
        case '#':  skip_comment();
                   result = fetch(input);
                   break;
        case '"':  result = parse_string(input);
                   break;
        case EOF:  break;
        default:   result = parse_token(input);
                   break;

    }
    return result;
}

static char* parse_string(FILE* input)
{
    size_t strsize  = 8;
    size_t strindex = 0;
    char*  string   = (char*)malloc(strsize);
    grow_token(&strsize, &strindex, &string, fgetc(input));
    while('"' != fpeekc(input))
    {
        if(feof(input)) {
            free(string);
            return NULL;
        }
        grow_token(&strsize, &strindex, &string, fgetc(input));
    }
    grow_token(&strsize, &strindex, &string, fgetc(input));
    return string;
}

static char* parse_token(FILE* input)
{
    size_t strsize  = 8;
    size_t strindex = 0;
    char*  string   = (char*)malloc(strsize);
    while(!is_whitespace(input))
    {
        if(feof(input)) {
            free(string);
            return NULL;
        }
        grow_token(&strsize, &strindex, &string, fgetc(input));
    }
    return string;
}

static void grow_token(size_t* p_size, size_t* p_index, char** p_p_str, char ch)
{
    /* If we don't have enough room for the new char */
    if( (*(p_index)+1) >= *p_size )
    {
        /* Double the string size */
        *(p_size) *= 2;
        *(p_p_str) = (char*)realloc( *(p_p_str), *(p_size) );
    }
    /* Add the new char and null terminate the string */
    *(p_p_str)[*(p_index)++] = ch;
    *(p_p_str)[*(p_index)]   = '\0';
}

static void skip_whitespace(FILE* input)
{
    while(!feof(input) && is_whitespace(input))
    {
        (void)fgetc(input);
    }
}

static void skip_comment(FILE* input)
{
    while(!feof(input) && ('\n' != fgetc(input)))
    {
    }
}

static bool is_whitespace(FILE* input)
{
    char ch = peekc(input);
    return ((ch == ' ')  || (ch == '\t') ||
            (ch == '\r') || (ch == '\n'));
}

static char fpeekc(FILE* input)
{
    char ch = fgetc(input);
    ungetc(ch,input);
    return ch;
}

/* Parsing Tokens
 *****************************************************************************/
static bool is_float(val_t* p_val);
static bool is_integer(val_t* p_val);
static bool is_string(val_t* p_val);
static bool is_char(val_t* p_val);

ValueType_T parse(char* str, val_t* p_val)
{
    ValueType_T type = ERROR;
    if(str != NULL)
    {
        if(!is_float(p_val) &&
           !is_integer(p_val) &&
           !is_string(p_val) &&
           !is_char(p_val))
        {
            type = WORD;
            *(p_val) = (val_t)str;
        }
    }
    return type;
}


static bool is_float(val_t* p_val)
{
    return false;
}

static bool is_integer(val_t* p_val)
{
    return false;
}

static bool is_string(val_t* p_val)
{
    return false;
}

static bool is_char(val_t* p_val)
{
    return false;
}


