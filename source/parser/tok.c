/**
    @file tok.c
    @brief See header for details
    $Revision$
    $HeadURL$
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tok.h"
#include "tokens.h"

#define FIELD_TYPE       0
#define FIELD_LINE_NUM   1
#define FIELD_COLUMN_NUM 2
#define FIELD_TEXT       3
#define FIELD_MAX        4

#define MAX_INPUT_STR 1024
FILE* Handle = NULL;
tok_t Token  = { 0 };
char  Buffer[MAX_INPUT_STR];

bool tok_source(char* fname)
{
    if (NULL == fname)
    {
        Handle = stdin;
    }
    else
    {
        Handle = fopen(fname,"r");
    }
    return (NULL != Handle);
}

bool tok_eof(void)
{
    bool ret = true;
    if (NULL != Handle)
    {
        ret = feof( Handle );
    }
    return ret;
}

void tok_finish(void)
{
    fclose(Handle);
}

tok_t* tok_read(void)
{
    tok_t* p_tok = NULL;
    if (NULL != Handle)
    {
        fgets(Buffer, MAX_INPUT_STR, Handle);
        if ('\0' != Buffer[0])
        {
            if ('@' == Buffer[0])
            {
                tok_read_fname();
                p_tok = tok_read();
            }
            else
            {
                p_tok = tok_build_token();
            }
            Buffer[0] = '\0';
        }
    }
    return p_tok;
}

void tok_read_fname(void)
{
    uint32_t index = 0;
    Buffer[strlen(Buffer)-1] = '\0';
    while(' ' != Buffer[index]) index++;
    char* new_str = (char*)malloc( strlen(&Buffer[index+1]) );
    strcpy( new_str, &Buffer[index+1] );
    Token.p_file_name = new_str;
}

tok_t* tok_build_token(void)
{
    tok_t* ret = NULL;
    uint32_t index;
    uint32_t start = 0;
    uint32_t end = 0;

    /* Look for and read all of the fields */
    for (index = 0; index < FIELD_MAX; index++)
    {
        /* Advance to the next field */
        bool last_field = tok_next_field( &end );
        /* copy the filed data */
        tok_read_field( index, &Buffer[start] );
        /* advance to next field or exit if last field */
        if (last_field) break;
        else start = ++end;
    }

    /* Copy the token to the heap */
    if (index == FIELD_TEXT)
    {
        ret = (tok_t*)calloc(1,sizeof(tok_t));
        *ret = Token;
    }

    return ret;
}

bool tok_next_field(uint32_t* end)
{
    bool last_field = false;
    while (('\t' != Buffer[*end]) &&
           !(('\r' == Buffer[*end]) ||
             ('\0' == Buffer[*end])))
    {
        (*end)++;
    }
    last_field = (('\r' == Buffer[*end]) ||
                  ('\0' == Buffer[*end])) ? true : false;
    Buffer[*end] = '\0';
    return last_field;
}

void tok_read_field(uint32_t index, char* str)
{
    uint32_t type;
    switch (index)
    {
        case FIELD_TYPE:
            for (type = 0; type < TOK_MAX; type++)
            {
                if (0 == strcmp(Token_Types[type],str))
                {
                    Token.type = (tok_type_t)type;
                    break;
                }
            }
            break;

        case FIELD_LINE_NUM:
            Token.line = atoi( str );
            break;

        case FIELD_COLUMN_NUM:
            Token.column = atoi( str );
            break;

        case FIELD_TEXT:
            Token.p_text = (char*)malloc(strlen(str));
            strcpy( Token.p_text, str );
            break;

        default:
            tok_fatal_error(1);
            break;
    }
}

void tok_fatal_error(uint32_t err_code)
{
    fprintf(stderr,"Fatal Error\n");
    exit(err_code);
}

