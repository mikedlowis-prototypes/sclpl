#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wordlist.h"

static void fetch_and_exec(void);
static word_t const* find_word(char const* name);
static void exec_word(word_t const* word);

long const* ArgStackPtr;

int main(int argc, char** argv)
{
    long stack[] = {0,42,0,0};
    ArgStackPtr = &stack[1];

    //exec_word(find_word("dup"));
    fetch_and_exec();

    printf("stack[3] = %d\n", (int)stack[3]);
    printf("stack[2] = %d\n", (int)stack[2]);
    printf("stack[1] = %d\n", (int)stack[1]);
    printf("stack[0] = %d\n", (int)stack[0]);
    return 0;
}

static void fetch_and_exec(void)
{
    char buffer[1024];
    printf("> ");
    fgets(buffer, 1024, stdin);
    find_word(buffer);
    puts(buffer);
}

static word_t const* find_word(char const* name) {
    word_t const* curr = LatestWord;
    while(curr)
    {
        if (0 == strcmp(curr->name,name))
        {
            break;
        }
        curr = curr->link;
    }
    return curr;
}

static void exec_word(word_t const* word) {
    word->codeword(word->code);
}

void exec_word_def(long const* code) {
    while(*code)
    {
        exec_word( (word_t const*)(*code) );
        code++;
    }
}

