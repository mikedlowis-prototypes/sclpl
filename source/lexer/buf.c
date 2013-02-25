#include <stdlib.h>
#include "buf.h"

size_t Size  = 0;
char* Buffer = NULL;
size_t Index = 0;

void buf_init(void)
{
    Size   = 16;
    Buffer = (char*)malloc(Size);
    Index  = 0;
    Buffer[Index] = '\0';
}

void buf_put(char ch)
{
    if ((Index + 2) >= Size)
    {
        buf_grow();
    }
    Buffer[Index++] = ch;
    Buffer[Index]   = '\0';
}

char* buf_accept(void)
{
    char* str = strdup(Buffer);
    Index = 0;
    Buffer[Index] = '\0';
    return str;
}

void buf_grow(void)
{
    Size = Size * 2;
    Buffer = realloc( Buffer, Size );
}

