#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "slvm.h"
#include "parser.h"

/*
    Wish List:

    * Rework Interpreter With Custom Syntax
    * Add Dictionary Typedef
    * Dynamic Loading and Unloading of Dictionaries (File, Object, or Dynamic Lib)
    * Rework w* words to allow gettign and setting attributes of a word
    * Add optional debugging words
    * Add optional floating point / fixed point words
    * Add optional runtime integrity checks
    * Add optional static heap replacement for malloc, realloc, free
    * Add optional support for embedded systems (No dependence on stdlib)
    * Run through profiler and optimize execution speed where possible.
    * Add optional support for temporaries
    * Add optional code optimizer for compiled words
    * Add ability to compile to simple object file
    * Add ability to compile to shared library
    * Add ability to compile to standalone executable
    * Add support for multi-tasking
    * Add support for multi-tasking with multiple cores/threads
*/

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

/* Inner Interpreter
 *****************************************************************************/
void docolon(val_t* code) {
    word_t* word;
    /* We may have previously been executing a word so we should save off our
     * previous position */
    val_t* prev_code = CodePtr;
    /* Set the next instruction to execute */
    CodePtr = code;
    /* And loop through until we "ret" sets the code pointer to null */
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
defconst("VERSION", version, 0, NULL,     1);
defconst("EXECDEF", execdef, 0, &version, (val_t)&docolon);
defconst("WORDSZ",  wordsz,  0, &execdef, sizeof(val_t));

/* Built-in Variables
 *****************************************************************************/
defvar("state",  state,  0, &wordsz, 0);
defvar("latest", latest, 0, &state,  0);

/* Word Words
 *****************************************************************************/
defcode("wlink", wlink, 0, &latest){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->link);
}

defcode("wsize", wflags, 0, &wlink){
    *(ArgStackPtr) = (val_t)(((word_t*)*(ArgStackPtr))->flags.attr.codesize);
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
    *(ArgStackPtr) = (val_t)((((word_t*)latest_val)->flags.attr.codesize) - 1);
}

/* Input/Output Words
 *****************************************************************************/
defvar("stdin",  _stdin,  0, &here,    0);
defvar("stdout", _stdout, 0, &_stdin,  0);
defvar("stderr", _stderr, 0, &_stdout, 0);

defconst("F_R",  f_r,  0, &_stderr, (val_t)"r");
defconst("F_W",  f_w,  0, &f_r,     (val_t)"w");
defconst("F_A",  f_a,  0, &f_w,     (val_t)"a");
defconst("F_R+", f_ru, 0, &f_a,     (val_t)"r+");
defconst("F_W+", f_wu, 0, &f_ru,    (val_t)"w+");
defconst("F_A+", f_au, 0, &f_wu,    (val_t)"a+");

defcode("fopen",  _fopen,  0, &f_au){
    *(ArgStackPtr-1) = (val_t)fopen( (const char*)*(ArgStackPtr-1), (const char*)*(ArgStackPtr) );
    ArgStackPtr--;
}

defcode("fclose", _fclose, 0, &_fopen){
    fclose((FILE*)*(ArgStackPtr));
    ArgStackPtr--;
}

defcode("fflush", _fflush, 0, &_fclose){
    fflush((FILE*)*(ArgStackPtr));
}

defcode("fgetc",  _fgetc,  0, &_fflush){
    ArgStackPtr++;
    *(ArgStackPtr) = fgetc((FILE*)*(ArgStackPtr-1));
}

defcode("fputc",  _fputc,  0, &_fgetc){
    fputc((char)*(ArgStackPtr), (FILE*)*(ArgStackPtr-1));
    ArgStackPtr--;
}

defcode("fpeekc", _fpeekc, 0, &_fputc){
    ArgStackPtr++;
    *(ArgStackPtr) = fgetc((FILE*)*(ArgStackPtr-1));
    ungetc((char)*(ArgStackPtr), (FILE*)*(ArgStackPtr-1));
}

/* Interpreter Words
 *****************************************************************************/
defcode("exec", exec, 0, &_fpeekc){
    word_t* word = (word_t*)(*ArgStackPtr);
    ArgStackPtr--;
    EXEC( *(word) );
}

defcode("find", find, 0, &exec){
    word_t const* curr = (word_t const*)latest_val;
    char* name = (char*)*(ArgStackPtr);
    while(curr)
    {
        if (!(curr->flags.attr.hidden) && (0 == strcmp(curr->name,name)))
        {
            break;
        }
        curr = curr->link;
    }
    *(ArgStackPtr) = (val_t)curr;
}

defcode("fetch", _fetch, 0, &find){
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)fetch_token((FILE*)_stdin_val);
}

defcode("parse", _parse, 0, &_fetch){
    char* p_str = (char*)*(ArgStackPtr);
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)parse( p_str, ArgStackPtr-1 );
    /* If the parsed token no longer needs the original string */
    if (*(ArgStackPtr) > STRING)
    {
        /* Free the mem */
        free(p_str);
    }
}

/* Branching and Literal Words
 *****************************************************************************/
defcode("lit", literal, 0, &_parse){
    ArgStackPtr++;
    *(ArgStackPtr) = *CodePtr;
    CodePtr++;
}

defcode("br", branch, 0, &literal){
    CodePtr = (val_t*)(((val_t)CodePtr) + (*(CodePtr) * sizeof(val_t)));
}

defcode("0br", zbranch, 0, &branch){
    if (*ArgStackPtr == 0)
    {
        CodePtr = (val_t*)(((val_t)CodePtr) + (*(CodePtr) * sizeof(val_t)));
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
    word->flags.attr.immed    = 0;
    word->flags.attr.hidden   = 1;
    word->flags.attr.codesize = 1;
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
    word->code[word->flags.attr.codesize-1] = *(ArgStackPtr);
    ArgStackPtr--;
    /* Resize the code section and relocate if necessary */
    word->flags.attr.codesize++;
    word->code = (val_t*)realloc(word->code, word->flags.attr.codesize * sizeof(val_t));
    /* Update "here" and terminate the code section */
    word->code[word->flags.attr.codesize-1] = (val_t)&ret;
}

defcode("hidden", hidden, 1, &comma){
    ((word_t*)*(ArgStackPtr))->flags.attr.hidden ^= 1;
}

defcode("immediate", immediate, 1, &hidden){
    ((word_t*)*(ArgStackPtr))->flags.attr.immed ^= 1;
}

defcode(":", colon, 0, &immediate){
    EXEC(_fetch);
    EXEC(_parse);
    ArgStackPtr--;
    EXEC(create);
    EXEC(rbrack);
}

defcode(";", semicolon, 1, &colon){
    EXEC(lbrack);
    EXEC(hidden);
    ArgStackPtr--;
}

defcode("'", tick, 1, &semicolon){
    //EXEC(get_word);
    //EXEC(find_word);
}

defcode("interp", interp, 0, &_parse){
    char* p_str = NULL;
    EXEC(_fetch);
    EXEC(_parse);
    /* If what we parsed was a word */
    if(*ArgStackPtr == WORD)
    {
        /* Consume the type token and save off the string pointer */
        ArgStackPtr--;
        p_str = (char*)*ArgStackPtr;
        /* Search for the word in the dictionary */
        EXEC(find);
        /* If we found a definition */
        if(*ArgStackPtr)
        {
            /* And the definition is marked immediate or we're in immediate mode */
            if((state_val == 0) || (((word_t*)*ArgStackPtr)->flags.attr.immed))
            {
                /* Execute it */
                EXEC(exec);
            }
            /* Otherwise, compile it! */
            else
            {
                EXEC(comma);
            }
        }
        /* We didn't find a definition */
        else
        {
            /* Ask the user what gives */
            printf("%s ?\n", p_str);
            /* Consume the token */
            ArgStackPtr--;
        }
    }
    /* What we parsed is a literal and we're in compile mode */
    else if (state_val == 1)
    {
        *(ArgStackPtr) = (val_t)&literal;
        EXEC(comma);
        EXEC(comma);
    }
    else
    {
        ArgStackPtr--;
    }

    /* If we saved off a pointer, we're done with it so free the memory */
    if(p_str) free(p_str);
}

defcode("quit", quit, 0, &interp){
    int i;
    printf("=> ");
    Line_Read = 0;
    while(1)
    {
        EXEC(interp);
        if(line_read())
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
        }

    }
}

/* Stack Manipulation Words
 *****************************************************************************/
defcode("drop", drop, 0, &tick){
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
    /* Compile branch instruction */
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)&zbranch;
    EXEC(comma);

    /* Save off the current offset */
    EXEC(here);

    /* Compile a dummy offset */
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)0;
    EXEC(comma);
}

defcode("then", _then, 1, &_if){
    /* calculate the address where the offset should be stored */
    EXEC(swap);
    EXEC(dup);
    EXEC(wcode);
    EXEC(nrot);

    /* Calculate the branch offset */
    EXEC(dup);
    EXEC(here);
    EXEC(swap);
    EXEC(sub);

    /* Store the offset */
    EXEC(swap);
    EXEC(wordsz);
    EXEC(mul);
    EXEC(nrot);
    EXEC(add);
    EXEC(store);
}

defcode("else", _else, 1, &_then){
    /* Compile the branch instruction */
    ArgStackPtr++;
    *(ArgStackPtr) = (val_t)&branch;
    EXEC(comma);

    /* Save off the current offset */
    EXEC(here);
    EXEC(rot);

    /* Compile a dummy offset */
    ArgStackPtr++;
    *(ArgStackPtr) = 0;
    EXEC(comma);

    /* Set the branch offset for the first branch */
    EXEC(_then);
    EXEC(swap);
}

/* Memory Management Words
 *****************************************************************************/
defcode("malloc", mem_alloc, 1, &_else){
    *(ArgStackPtr) = (val_t)malloc((size_t)*(ArgStackPtr));
}

defcode("mrealloc", mem_realloc, 1, &mem_alloc){
    *(ArgStackPtr-1) = (val_t)realloc((void*)*(ArgStackPtr-1),*(ArgStackPtr));
    ArgStackPtr--;
}

defcode("mfree", mem_free, 1, &mem_realloc){
    free((void*)*(ArgStackPtr));
    ArgStackPtr--;
}

/* Debugging Words
 *****************************************************************************/
defcode("printw", printw, 0, &mem_free){
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
            else if (*bytecode == (val_t)&branch)
            {
                bytecode++;
                printf("\tbr %ld\n", *bytecode);
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
               word->flags.attr.immed,
               word->flags.attr.hidden);
        word = word->link;
    }
}

/* Main
 *****************************************************************************/
int main(int argc, char** argv)
{
    CT_ASSERT(sizeof(val_t) == sizeof(val_t*));
    CT_ASSERT(sizeof(val_t) == sizeof(flags_t));
    _stdin_val  = (val_t)stdin;
    _stdout_val = (val_t)stdout;
    _stderr_val = (val_t)stderr;
    latest_val = (val_t)&printdefw;
    EXEC(quit);
    return 0;
}

