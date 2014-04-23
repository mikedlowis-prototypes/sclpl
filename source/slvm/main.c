#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "slvm.h"

/* Built-in Constants
 *****************************************************************************/
/** The argument stack */
val_t ArgStack[ARG_STACK_SIZE];

/** Pointer to current position on the stack */
val_t* ArgStackPtr = ArgStack-1;

/** Pointer to current instruction being executed */
val_t* CodePtr = 0;

/** A state variable used to flag when the interpreter reads a line of input */
val_t Line_Read = 0;

/* Built-in Constants
 *****************************************************************************/
void docolon(val_t* code) {
    word_t* word;
    /* We may have previously been executing a word so we should save off our
     * previous position */
    val_t* prev_code = CodePtr;
    /* Set the next instruction to execute */
    CodePtr = code;
    /* And loop through until we get the bytecode instruction of 0 (NEXT) */
    while(CodePtr)
    {
        word = (word_t*)*CodePtr;
        /* Increment the instruction pointer */
        CodePtr++;
        /* Execute the byte code instruction */
        word->codeword(CodePtr);
    }
    /* Execution finished lets put things back the way they were */
    CodePtr = prev_code;
}

/* Built-in Constants
 *****************************************************************************/
defconst("VERSION", version, 0, 0,        1);
defconst("EXECDEF", execdef, 0, &version, (val_t)&docolon);
defconst("WORDSZ",  wordsz,  0, &execdef, sizeof(val_t));

/* Built-in Variables
 *****************************************************************************/
defvar("state",  state,  0, &wordsz, 0);
//defvar("here",   here,   0, &state,  0);
defvar("latest", latest, 0, &state,   0);

/* Word Words
 *****************************************************************************/
defcode("wlink", wlink, 0, &latest){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->link);
}

defcode("wsize", wflags, 0, &wlink){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->flags.codesize);
}

defcode("wname", wname, 0, &wflags){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->name);
}

defcode("wfunc", wfunc, 0, &wname){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->codeword);
}

defcode("wcode", wcode, 0, &wfunc){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->code);
}

defcode("here", here, 0, &wcode){
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)((((word_t*)latest_val)->flags.codesize) - 1);
}

/* Input/Output Words
 *****************************************************************************/

/* Input Words
 *****************************************************************************/
defcode("getc", get_char, 0, &here){
    ArgStackPtr++;
    *(ArgStackPtr) = getc(stdin);
}

defcode("ws?", is_ws, 0, &get_char){
    char ch = *(ArgStackPtr);
    *(ArgStackPtr) = ((ch == ' ')  || (ch == '\t') ||
                      (ch == '\r') || (ch == '\n'));

    /* TODO: total hack to get the prompt to reappear when the user hits
     * enter */
    if(ch == '\n')
        Line_Read = 1;
}

defcode("getw", get_word, 0, &is_ws){
    static char buffer[32];
    int i = 0;
    int wschar = 0;

    /* Skip any whitespace */
    do {
        EXEC(get_char);
        buffer[i] = (char)*(ArgStackPtr);
        EXEC(is_ws);
        wschar = *(ArgStackPtr);
        ArgStackPtr--;
    } while(wschar);

    /* Read the rest of the word */
    while(!wschar)
    {
        i++;
        EXEC(get_char);
        buffer[i] = (char)*(ArgStackPtr);
        EXEC(is_ws);
        wschar = *(ArgStackPtr);
        ArgStackPtr--;
    }

    /* Terminate the string */
    buffer[i] = '\0';

    /* Return the word */
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)&buffer;
}

defcode("findw", find_word, 0, &get_word){
    word_t const* curr = (word_t const*)latest_val;
    char* name = (char*)*(ArgStackPtr);
    while(curr)
    {
        if (!(curr->flags.f_hidden) && (0 == strcmp(curr->name,name)))
        {
            break;
        }
        curr = curr->link;
    }
    *(ArgStackPtr) = (val_t)curr;
}

/* Branching and Literal Words
 *****************************************************************************/
defcode("lit", literal, 0, &find_word){
    ArgStackPtr++;
    *(ArgStackPtr) = *CodePtr;
    CodePtr++;
}

defcode("br", branch, 0, &literal){
    CodePtr = (val_t*)(((val_t)CodePtr) + *(CodePtr));
}

defcode("0br", zbranch, 0, &branch){
    if (*ArgStackPtr == 0)
    {
        CodePtr = (val_t*)(((val_t)CodePtr) + *(CodePtr));
    }
    else
    {
        CodePtr++;
    }
    ArgStackPtr--;
}

/* Compiler Words
 *****************************************************************************/
defcode("ret", ret, 0, &zbranch){
    CodePtr = 0;
}

defcode("[", lbrack, 0, &ret){
    state_val = 0;
}

defcode("]", rbrack, 1, &lbrack){
    state_val = 1;
}

defcode("create", create, 0, &rbrack){
    /* Copy the name string */
    char* name = 0u;
    if (*(ArgStackPtr))
    {
        size_t namesz = strlen((char*)*(ArgStackPtr));
        name = (char*)malloc( namesz );
        strcpy(name, (char*)*(ArgStackPtr));
    }
    /* Create the word entry */
    word_t* word   = (word_t*)malloc(sizeof(word_t));
    word->link     = (word_t*)latest_val;
    /* Initialize the flags (hidden and non-immediate by default) */
    word->flags.f_immed  = 0;
    word->flags.f_hidden = 1;
    word->flags.codesize = 1;
    /* Initialize the name, codeword, and bytecode */
    word->name     = name;
    word->codeword = &docolon;
    word->code     = (val_t*)malloc(sizeof(val_t));
    word->code[0]  = (val_t)&ret;
    /* Update Latest and Return the new word */
    latest_val     = (val_t)word;
    *(ArgStackPtr) = (val_t)word;
}

defcode(",", comma, 0, &create){
    /* Get the word we are currently compiling */
    word_t* word  = (word_t*)latest_val;
    /* Put the next instruction in place of the terminating 'ret' that "here"
     * points too */
    word->code[word->flags.codesize-1] = *(ArgStackPtr);
    ArgStackPtr--;
    /* Resize the code section and relocate if necessary */
    word->flags.codesize++;
    word->code = (val_t*)realloc(word->code, word->flags.codesize * sizeof(val_t));
    /* Update "here" and terminate the code section */
    word->code[word->flags.codesize-1] = (val_t)&ret;
}

defcode("hidden", hidden, 1, &comma){
    ((word_t*)*(ArgStackPtr))->flags.f_hidden ^= 1;
}

defcode("immediate", immediate, 1, &hidden){
    ((word_t*)*(ArgStackPtr))->flags.f_immed ^= 1;
}

defcode(":", colon, 0, &immediate){
    EXEC(get_word);
    EXEC(create);
    EXEC(rbrack);
}

defcode(";", semicolon, 1, &colon){
    EXEC(lbrack);
    EXEC(hidden);
    ArgStackPtr--;
}

defcode("'", tick, 1, &semicolon){
    EXEC(get_word);
    EXEC(find_word);
}

/* Interpreter Words
 *****************************************************************************/
defcode("execw", exec_word, 0, &tick){
    word_t* word = (word_t*)(*ArgStackPtr);
    ArgStackPtr--;
    EXEC( *(word) );
}

defcode("parsenum", parse_num, 0, &exec_word){
    char* end;
    val_t num = strtol((const char *)*(ArgStackPtr), &end, 10);
    if(end != (char *)*(ArgStackPtr))
    {
        *(ArgStackPtr) = num;
    }
    else
    {
        printf("%s ? \n", ((char*)*(ArgStackPtr)));
        ArgStackPtr--;
    }
}

defcode("interpret", interpret, 0, &parse_num){
    char* curr_word;
    /* Parse a word */
    EXEC(get_word);
    curr_word = (char*)*(ArgStackPtr);
    /* Find the word */
    EXEC(find_word);

    /* if we found a word */
    if (*ArgStackPtr)
    {
        /* If we are in immediate mode or the found word is marked immediate */
        if((state_val == 0) || (((word_t*)*ArgStackPtr)->flags.f_immed))
        {
            /* Execute the word */
            EXEC(exec_word);
        }
        /* else we are compiling */
        else
        {
            EXEC(comma);
        }
    }
    /* else parse it as a number */
    else
    {
        *(ArgStackPtr) = (val_t)curr_word;
        EXEC(parse_num);
        if (state_val == 1)
        {
            ArgStackPtr++;
            *(ArgStackPtr) = (val_t)&literal;
            EXEC(comma);
            EXEC(comma);
        }
        else if (errno == ERANGE)
        {
            ArgStackPtr--;
        }
    }
}

defcode("quit", quit, 0, &interpret){
    int i;
    printf("=> ");
    Line_Read = 0;
    while(1)
    {
        EXEC(interpret);
        if(Line_Read)
        {
            val_t stacksz = ArgStackPtr - ArgStack + 1;
            if (stacksz > 5)
                printf("( ... ");
            else
                printf("( ");

            for(i = (stacksz > 5) ? 4 : stacksz-1; i >= 0; i--)
            {
                printf("%ld ", *(ArgStackPtr-i));
            }
            printf(")\n%s ", (state_val == 0) ? "=>" : "..");
            Line_Read = 0;
        }

    }
}

/* Stack Manipulation Words
 *****************************************************************************/
defcode("drop", drop, 0, &quit){
    ArgStackPtr--;
}

defcode("swap", swap, 0, &drop){
    val_t temp = *(ArgStackPtr);
    *(ArgStackPtr) = *(ArgStackPtr-1);
    *(ArgStackPtr-1) = temp;
}

defcode("dup", dup, 0, &swap){
    ArgStackPtr++;
    *(ArgStackPtr) = *(ArgStackPtr-1);
}

defcode("over", over, 0, &dup){
    ArgStackPtr++;
    *(ArgStackPtr) = *(ArgStackPtr-2);
}

defcode("rot", rot, 0, &over){
    val_t temp = *(ArgStackPtr);
    *(ArgStackPtr) = *(ArgStackPtr-1);
    *(ArgStackPtr-1) = *(ArgStackPtr-2);
    *(ArgStackPtr-2) = temp;
}

defcode("-rot", nrot, 0, &rot){
    val_t temp = *(ArgStackPtr-2);
    *(ArgStackPtr-2) = *(ArgStackPtr-1);
    *(ArgStackPtr-1) = *(ArgStackPtr);
    *(ArgStackPtr) = temp;
}

/* Arithmetic Words
 *****************************************************************************/
defcode("+", add, 0, &nrot){
    *(ArgStackPtr-1) += *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("-", sub, 0, &add){
    *(ArgStackPtr-1) -= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("*", mul, 0, &sub){
    *(ArgStackPtr-1) *= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("/", divide, 0, &mul){
    *(ArgStackPtr-1) /= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("%", mod, 0, &divide){
    *(ArgStackPtr-1) %= *(ArgStackPtr);
    ArgStackPtr--;
}

/* Boolean Conditional Words
 *****************************************************************************/
defcode("=", equal, 0, &mod){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) == *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("!=", notequal, 0, &equal){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) != *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("<", lessthan, 0, &notequal){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) < *(ArgStackPtr);
    ArgStackPtr--;
}

defcode(">", greaterthan, 0, &lessthan){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) > *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("<=", lessthaneq, 0, &greaterthan){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) <= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode(">=", greaterthaneq, 0, &lessthaneq){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) >= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("and", and, 0, &greaterthaneq){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) && *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("or", or, 0, &and){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) || *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("not", not, 0, &or){
    *(ArgStackPtr) = !(*(ArgStackPtr));
}

/* Bitwise Words
 *****************************************************************************/
defcode("band", band, 0, &not){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) & *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("bor", bor, 0, &band){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) | *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("bxor", bxor, 0, &bor){
    *(ArgStackPtr-1) = *(ArgStackPtr-1) ^ *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("bnot", bnot, 0, &bxor){
    *(ArgStackPtr) = ~(*(ArgStackPtr));
}

/* Memory Manipulation Words
 *****************************************************************************/
defcode("!", store, 0, &bnot){
    *((val_t*)*(ArgStackPtr)) = *(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("@", fetch, 0, &store){
    *(ArgStackPtr) = *((val_t*)*(ArgStackPtr));
}

defcode("+!", addstore, 0, &fetch){
    *((val_t*)*(ArgStackPtr)) += *(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("-!", substore, 0, &addstore){
    *((val_t*)*(ArgStackPtr)) -= *(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("b!", bytestore, 0, &substore){
    *((char*)*(ArgStackPtr)) = (char)*(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("b@", bytefetch, 0, &bytestore){
    *(ArgStackPtr) = *((char*)*(ArgStackPtr));
}

defcode("b@b!", bytecopy, 0, &bytefetch){
}

defcode("bmove", bytemove, 0, &bytecopy){
}

/* Control Flow Words
 *****************************************************************************/
defcode("if", _if, 1, &bytemove){
    // : IF IMMEDIATE
    //         ' 0BRANCH ,     \ compile 0BRANCH
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)&zbranch;
    EXEC(comma);
    //         HERE @          \ save location of the offset on the stack
    //EXEC(here);
    //ArgStackPtr++;
    //*(ArgStackPtr) = here_val;
    //         0 ,             \ compile a dummy offset
    //ArgStackPtr++;
    //*(ArgStackPtr) = 0;
    //EXEC(comma);
    // ;
}

defcode("then", _then, 1, &_if){
    // : THEN IMMEDIATE
    //         DUP
    EXEC(dup);
    //         HERE @ SWAP -   \ calculate the offset from the address saved on the stack
    //ArgStackPtr++;
    //*(ArgStackPtr) = here_val;
    EXEC(here);
    EXEC(swap);
    EXEC(sub);
    //         SWAP !          \ store the offset in the back-filled location
    EXEC(swap);
    *((val_t*)*ArgStackPtr) = *(ArgStackPtr-1);
    ArgStackPtr -= 2;
    // ;
}

defcode("else", _else, 1, &_then){
    // : ELSE IMMEDIATE
    //         ' BRANCH ,      \ definite branch to just over the false-part
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)&branch;
    EXEC(comma);
    //         HERE @          \ save location of the offset on the stack
    //ArgStackPtr++;
    //*(ArgStackPtr) = here_val;
    EXEC(here);
    //         0 ,             \ compile a dummy offset
    ArgStackPtr++;
    *(ArgStackPtr) = 0;
    EXEC(comma);
    //         SWAP            \ now back-fill the original (IF) offset
    EXEC(swap);
    //         DUP             \ same as for THEN word above
    EXEC(dup);
    //         HERE @ SWAP -
    //ArgStackPtr++;
    //*(ArgStackPtr) = here_val;
    EXEC(swap);
    EXEC(sub);
    //         SWAP !
    EXEC(swap);
    *((val_t*)*ArgStackPtr) = *(ArgStackPtr-1);
    ArgStackPtr -= 2;
    // ;
}

/* Debugging Words
 *****************************************************************************/
defcode("printw", printw, 0, &_else){
    word_t* word = (word_t*)*(ArgStackPtr);
    val_t* bytecode = word->code;
    ArgStackPtr--;

    printf("Name:     %s\n", word->name);
    //printf("Flags:    0x%lX\n", word->flags);
    if (word->codeword == &docolon)
    {
        puts("CodeFn:   docolon");
        puts("Bytecode:");
        while(bytecode)
        {
            if (*bytecode == (val_t)&literal)
            {
                bytecode++;
                printf("\tlit %ld\n", *bytecode);
            }
            else if (*bytecode == (val_t)&zbranch)
            {
                bytecode++;
                printf("\t0br %ld\n", *bytecode);
            }
            else
            {
                printf("\t%s\n", ((word_t*) *bytecode)->name);
            }

            if (*bytecode == (val_t)&ret)
            {
                bytecode = 0;
                break;
            }
            else
            {
                bytecode++;
            }
        }
    }
    else
    {
        printf("CodeFn:   0x%lX\n",(val_t)word->codeword);
        printf("Bytecode: (native)\n");
    }
}

defcode("printallw", printallw, 0, &printw){
    const word_t* word = (word_t*)latest_val;
    while(word)
    {
        puts(word->name);
        word = word->link;
    }
}

defcode("printdefw", printdefw, 0, &printallw){
    const word_t* word = (word_t*)latest_val;
    while(word != &printdefw)
    {
        printf("%s\t%ld %ld",
               word->name,
               word->flags.f_immed,
               word->flags.f_hidden);
        word = word->link;
    }
}

/* Main
 *****************************************************************************/
int main(int argc, char** argv)
{
    CT_ASSERT(sizeof(val_t) == sizeof(val_t*));
    latest_val = (val_t)&printdefw;
    EXEC(quit);
    return 0;
}

