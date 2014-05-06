/**
  @file pal.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include "../../pal.h"
#include <stdio.h>
#include <stdlib.h>

static val_t Stack[ARG_STACK_SIZE];
static bool Line_Read = true;
val_t* ArgStack = Stack - 1;
val_t* CodePtr = 0;

dict_t* pal_init(dict_t* p_prev_dict)
{
    return p_prev_dict;
}

void pal_prompt(void)
{
    int i;
    if(Line_Read)
    {
        val_t stacksz = ArgStack - Stack + 1;
        if (stacksz > 5)
            printf("( ... ");
        else
            printf("( ");

        for(i = (stacksz > 5) ? 4 : stacksz-1; i >= 0; i--)
        {
            printf("%ld ", *(ArgStack-i));
        }
        printf(")\n%s ", "=>"); //(state_val == 0) ? "=>" : "..");
        Line_Read = false;
    }
}

void pal_unknown_word(char* p_str)
{
    printf("%s ?\n", p_str);
}

void* pal_allocate(size_t size)
{
    return malloc(size);
}

void* pal_reallocate(void* p_mem, size_t size)
{
    return realloc(p_mem, size);
}

void pal_free(void* p_mem)
{
    free(p_mem);
}

char pal_read_char(void)
{
    return fgetc(stdin);
}

char pal_peek_char(void)
{
    char ch = fgetc(stdin);
    ungetc(ch,stdin);
    if ((ch == '\n') || (ch == '\r'))
    {
        Line_Read = true;
    }
    return ch;
}

bool pal_is_eof(void)
{
    return feof(stdin);
}


