#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wordlist.h"

static word_t* find_word(char* name);
static void exec_word(word_t* word);

long* ArgStackPtr;
long* InstructionPtr;

int main(int argc, char** argv)
{
    long stack[] = {0,42,0,0};
    ArgStackPtr = &stack[1];

    //exec_word(find_word("dupdup"));
    exec_word(find_word("dup"));

    printf("stack[3] = %d\n", (int)stack[3]);
    printf("stack[2] = %d\n", (int)stack[2]);
    printf("stack[1] = %d\n", (int)stack[1]);
    printf("stack[0] = %d\n", (int)stack[0]);
    return 0;
}

static word_t* find_word(char* name) {
    word_t* curr = LatestWord;
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

static void exec_word(word_t* word) {
    word->codeword(word->code);
}

void exec_word_def(long* code) {
    while(*code)
    {
        exec_word( (word_t*)(*code) );
        code++;
    }
}

