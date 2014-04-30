/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "parser.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Fetching Tokens
 *****************************************************************************/
static void skip_whitespace(FILE* input);
static void skip_comment(FILE* input);
static char* read_string(FILE* input);
static char* read_token(FILE* input);
//static void grow_token(size_t* p_size, size_t* p_index, char** p_p_str, char ch);
static char* grow_token(size_t* p_size, size_t* p_index, char* p_str, char ch);
static bool is_whitespace(FILE* input);
static char fpeekc(FILE* input);

char* fetch_token(FILE* input)
{
    char* result = NULL;
    puts("skipping whitespace");
    skip_whitespace(input);
    puts("reading token");
    switch(fpeekc(input))
    {
        case '#':
            skip_comment(input);
            result = fetch_token(input);
            break;

        case '"':
            result = read_string(input);
            break;

        case EOF:
            break;

        default:
            puts("reading word num or char");
            result = read_token(input);
            break;

    }
    puts("returning token");
    return result;
}

static char* read_string(FILE* input)
{
    size_t strsize  = 8;
    size_t strindex = 0;
    char*  string   = (char*)malloc(strsize);

    string = grow_token(&strsize, &strindex, string, fgetc(input));
    while('"' != fpeekc(input))
    {
        if(feof(input)) {
            free(string);
            return NULL;
        }
        string = grow_token(&strsize, &strindex, string, fgetc(input));
    }
    string = grow_token(&strsize, &strindex, string, fgetc(input));

    return string;
}

static char* read_token(FILE* input)
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
        string = grow_token(&strsize, &strindex, string, fgetc(input));
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
        p_str = (char*)realloc( p_str, *(p_size) );
    }
    /* Set the current char to the provided value and null terminate the str */
    p_str[ *(p_index) ]  = ch;
    p_str[ *(p_index)+1] = '\0';
    *(p_index) += 1;
    /* Return the (possibly moved) string */
    return p_str;
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
    char ch = fpeekc(input);
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
//static bool is_float(val_t* p_val);
static bool is_integer(val_t* p_val);
//static bool is_string(val_t* p_val);
//static bool is_char(val_t* p_val);

TokenType_T parse(char* str, val_t* p_val)
{
    TokenType_T type = ERROR;
    if(str != NULL)
    {
        puts(str);
        if(is_integer(p_val))
        {
            type = INTEGER;
        }
        else
        {
            type = WORD;
            *(p_val) = (val_t)str;
        }
        //if(!is_float(p_val) &&
        //   !is_integer(p_val) &&
        //   !is_string(p_val) &&
        //   !is_char(p_val))
        //{
        //    type = WORD;
        //    *(p_val) = (val_t)str;
        //}
    }
    return type;
}


//static bool is_float(val_t* p_val)
//{
//    return false;
//}

static bool is_integer(val_t* p_val)
{
    char* str = (char*)(*p_val);
    char* end;
    *(p_val) = (val_t)strtol(str,&end,0);
    printf("%#x == %#x\n", end, &(str[strlen(str)-1]));
    return (end == &(str[strlen(str)-1]));
}

//static bool is_string(val_t* p_val)
//{
//    return false;
//}

//static bool is_char(val_t* p_val)
//{
//    return false;
//}


