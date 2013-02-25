#include <stdio.h>
#include "file.h"

int Line     = 1;
int Column   = 1;
char* Name   = NULL;
FILE* Handle = NULL;

bool file_open(char* fname)
{
    Line   = 1;
    Column = 1;
    Name   = fname;
    if (NULL == Name)
    {
        Handle = stdin;
    }
    else
    {
        printf("%s\n",fname);
        Handle = fopen(fname,"r");
    }
    return (NULL != Handle);
}

void file_close(void)
{
    fclose(Handle);
}

bool file_eof(void)
{
    bool ret = true;
    if (NULL != Handle)
    {
        ret = feof( Handle );
    }
    return ret;
}

char file_get(void)
{
    char ret = EOF;
    if (NULL != Handle)
    {
        ret = fgetc(Handle);
        if ('\n' == ret)
        {
            Line++;
            Column = 1;
        }
        else
        {
            Column++;
        }
    }
    return ret;
}

char file_peek(void)
{
    char ret = fgetc(Handle);
    if (EOF != ret)
    {
        ungetc(ret,Handle);
    }
    return ret;
}

int file_line(void)
{
    return Line;
}

int file_column(void)
{
    return Column;
}

char* file_name(void)
{
    return (NULL != Name) ? Name : "<stdin>";
}

