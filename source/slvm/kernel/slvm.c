#include "slvm.h"
#include "parser.h"
#include "pal.h"

/*
    Wish List:

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

/* Inner Interpreter
 *****************************************************************************/
void docolon(val_t* code) {
    word_t* word;
    /* We may have previously been executing a word so we should save off our
     * previous position */
    val_t* prev_code = CodePtr;
    /* Set the next instruction to execute */
    CodePtr = code;
    /* Loop through until "ret" sets the code pointer to null */
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
defconst("VERSION", version, 0, NULL,     1)
defconst("EXECDEF", execdef, 0, &version, (val_t)&docolon)
defconst("WORDSZ",  wordsz,  0, &execdef, sizeof(val_t))

/* Built-in Variables
 *****************************************************************************/
defvar("state",  state,  0, &wordsz, 0)
defvar("latest", latest, 0, &state,  0)

/* Word Words
 *****************************************************************************/
defcode("wlink", wlink, 0, &latest){
    *(ArgStack) = (val_t)(((word_t*)*(ArgStack))->link);
}

defcode("wsize", wflags, 0, &wlink){
    *(ArgStack) = (val_t)(((word_t*)*(ArgStack))->flags.attr.codesize);
}

defcode("wname", wname, 0, &wflags){
    *(ArgStack) = (val_t)(((word_t*)*(ArgStack))->name);
}

defcode("wfunc", wfunc, 0, &wname){
    *(ArgStack) = (val_t)(((word_t*)*(ArgStack))->codeword);
}

defcode("wcode", wcode, 0, &wfunc){
    *(ArgStack) = (val_t)(((word_t*)*(ArgStack))->code);
}

defcode("here", here, 0, &wcode){
    ArgStack++;
    *(ArgStack) = (val_t)((((word_t*)latest_val)->flags.attr.codesize) - 1);
}

/* Interpreter Words
 *****************************************************************************/
defcode("exec", exec, 0, &here){
    word_t* word = (word_t*)(*ArgStack);
    ArgStack--;
    EXEC( *(word) );
}

defcode("find", find, 0, &exec){
    word_t const* curr = (word_t const*)latest_val;
    char* name = (char*)*(ArgStack);
    while(curr)
    {
        if (!(curr->flags.attr.hidden) && (0 == pal_strcmp(curr->name,name)))
        {
            break;
        }
        curr = curr->link;
    }
    *(ArgStack) = (val_t)curr;
}

defcode("fetchtok", fetchtok, 0, &find){
    ArgStack++;
    *(ArgStack) = (val_t)fetch_token();
}

defcode("parsetok", parsetok, 0, &fetchtok){
    char* p_str = (char*)*(ArgStack);
    ArgStack++;
    *(ArgStack) = (val_t)parse_token( p_str, ArgStack-1 );
    /* If the parsed token no longer needs the original string */
    if (*(ArgStack) > STRING)
    {
        /* Free the mem */
        pal_free(p_str);
    }
}

/* Branching and Literal Words
 *****************************************************************************/
defcode("lit", lit, 0, &parsetok){
    ArgStack++;
    *(ArgStack) = *CodePtr;
    CodePtr++;
}

defcode("br", br, 0, &lit){
    CodePtr = (val_t*)(((val_t)CodePtr) + (*(CodePtr) * sizeof(val_t)));
}

defcode("0br", zbr, 0, &br){
    if (*ArgStack == 0)
    {
        CodePtr = (val_t*)(((val_t)CodePtr) + (*(CodePtr) * sizeof(val_t)));
    }
    else
    {
        CodePtr++;
    }
    ArgStack--;
}

/* Compiler Words
 *****************************************************************************/
defcode("ret", ret, 0, &zbr){
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
    if (*(ArgStack))
    {
        size_t namesz = pal_strlen((char*)*(ArgStack));
        name = (char*)pal_allocate( namesz );
        pal_strcpy(name, (char*)*(ArgStack));
    }
    /* Create the word entry */
    word_t* word   = (word_t*)pal_allocate(sizeof(word_t));
    word->link     = (word_t*)latest_val;
    /* Initialize the flags (hidden and non-immediate by default) */
    word->flags.attr.immed    = 0;
    word->flags.attr.hidden   = 1;
    word->flags.attr.codesize = 1;
    /* Initialize the name, codeword, and bytecode */
    word->name     = name;
    word->codeword = &docolon;
    word->code     = (val_t*)pal_allocate(sizeof(val_t));
    word->code[0]  = (val_t)&ret;
    /* Update Latest and Return the new word */
    latest_val     = (val_t)word;
    *(ArgStack) = (val_t)word;
}

defcode(",", comma, 0, &create){
    /* Get the word we are currently compiling */
    word_t* word  = (word_t*)latest_val;
    /* Put the next instruction in place of the terminating 'ret' that "here"
     * points too */
    word->code[word->flags.attr.codesize-1] = *(ArgStack);
    ArgStack--;
    /* Resize the code section and relocate if necessary */
    word->flags.attr.codesize++;
    word->code = (val_t*)pal_reallocate(word->code, word->flags.attr.codesize * sizeof(val_t));
    /* Update "here" and terminate the code section */
    word->code[word->flags.attr.codesize-1] = (val_t)&ret;
}

defcode("hidden", hidden, 1, &comma){
    ((word_t*)*(ArgStack))->flags.attr.hidden ^= 1;
}

defcode("immediate", immediate, 1, &hidden){
    ((word_t*)*(ArgStack))->flags.attr.immed ^= 1;
}

defcode(":", colon, 0, &immediate){
    EXEC(fetchtok);
    EXEC(parsetok);
    ArgStack--;
    EXEC(create);
    EXEC(rbrack);
}

defcode(";", semicolon, 1, &colon){
    EXEC(lbrack);
    EXEC(hidden);
    ArgStack--;
}

defcode("'", tick, 1, &semicolon){
    EXEC(fetchtok);
    EXEC(parsetok);
    ArgStack--;
    EXEC(find);
}

defcode("interp", interp, 0, &parsetok){
    char* p_str = NULL;
    EXEC(fetchtok);
    EXEC(parsetok);
    /* If what we parsed was a word */
    if(*ArgStack == WORD)
    {
        /* Consume the type token and save off the string pointer */
        ArgStack--;
        p_str = (char*)*ArgStack;
        /* Search for the word in the dictionary */
        EXEC(find);
        /* If we found a definition */
        if(*ArgStack)
        {
            /* And the definition is marked immediate or we're in immediate mode */
            if((state_val == 0) || (((word_t*)*ArgStack)->flags.attr.immed))
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
            pal_unknown_word(p_str);
            /* Consume the token */
            ArgStack--;
        }
    }
    /* What we parsed is a literal and we're in compile mode */
    else if (state_val == 1)
    {
        *(ArgStack) = (val_t)&lit;
        EXEC(comma);
        EXEC(comma);
    }
    else
    {
        ArgStack--;
    }

    /* If we saved off a pointer, we're done with it so free the memory */
    if(p_str) pal_free(p_str);
}

defcode("quit", quit, 0, &interp){
    while(1)
    {
        pal_prompt();
        EXEC(interp);
    }
}

/* Stack Manipulation Words
 *****************************************************************************/
defcode("drop", drop, 0, &tick){
    ArgStack--;
}

defcode("swap", swap, 0, &drop){
    val_t temp = *(ArgStack);
    *(ArgStack) = *(ArgStack-1);
    *(ArgStack-1) = temp;
}

defcode("dup", dup, 0, &swap){
    ArgStack++;
    *(ArgStack) = *(ArgStack-1);
}

defcode("over", over, 0, &dup){
    ArgStack++;
    *(ArgStack) = *(ArgStack-2);
}

defcode("rot", rot, 0, &over){
    val_t temp = *(ArgStack);
    *(ArgStack) = *(ArgStack-1);
    *(ArgStack-1) = *(ArgStack-2);
    *(ArgStack-2) = temp;
}

defcode("-rot", nrot, 0, &rot){
    val_t temp = *(ArgStack-2);
    *(ArgStack-2) = *(ArgStack-1);
    *(ArgStack-1) = *(ArgStack);
    *(ArgStack) = temp;
}

/* Arithmetic Words
 *****************************************************************************/
defcode("+", add, 0, &nrot){
    *(ArgStack-1) += *(ArgStack);
    ArgStack--;
}

defcode("-", sub, 0, &add){
    *(ArgStack-1) -= *(ArgStack);
    ArgStack--;
}

defcode("*", mul, 0, &sub){
    *(ArgStack-1) *= *(ArgStack);
    ArgStack--;
}

defcode("/", div, 0, &mul){
    *(ArgStack-1) /= *(ArgStack);
    ArgStack--;
}

defcode("%", mod, 0, &div){
    *(ArgStack-1) %= *(ArgStack);
    ArgStack--;
}

/* Boolean Conditional Words
 *****************************************************************************/
defcode("=", equal, 0, &mod){
    *(ArgStack-1) = *(ArgStack-1) == *(ArgStack);
    ArgStack--;
}

defcode("!=", notequal, 0, &equal){
    *(ArgStack-1) = *(ArgStack-1) != *(ArgStack);
    ArgStack--;
}

defcode("<", lessthan, 0, &notequal){
    *(ArgStack-1) = *(ArgStack-1) < *(ArgStack);
    ArgStack--;
}

defcode(">", greaterthan, 0, &lessthan){
    *(ArgStack-1) = *(ArgStack-1) > *(ArgStack);
    ArgStack--;
}

defcode("<=", lessthaneq, 0, &greaterthan){
    *(ArgStack-1) = *(ArgStack-1) <= *(ArgStack);
    ArgStack--;
}

defcode(">=", greaterthaneq, 0, &lessthaneq){
    *(ArgStack-1) = *(ArgStack-1) >= *(ArgStack);
    ArgStack--;
}

defcode("and", and, 0, &greaterthaneq){
    *(ArgStack-1) = *(ArgStack-1) && *(ArgStack);
    ArgStack--;
}

defcode("or", or, 0, &and){
    *(ArgStack-1) = *(ArgStack-1) || *(ArgStack);
    ArgStack--;
}

defcode("not", not, 0, &or){
    *(ArgStack) = !(*(ArgStack));
}

/* Bitwise Words
 *****************************************************************************/
defcode("band", band, 0, &not){
    *(ArgStack-1) = *(ArgStack-1) & *(ArgStack);
    ArgStack--;
}

defcode("bor", bor, 0, &band){
    *(ArgStack-1) = *(ArgStack-1) | *(ArgStack);
    ArgStack--;
}

defcode("bxor", bxor, 0, &bor){
    *(ArgStack-1) = *(ArgStack-1) ^ *(ArgStack);
    ArgStack--;
}

defcode("bnot", bnot, 0, &bxor){
    *(ArgStack) = ~(*(ArgStack));
}

/* Memory Manipulation Words
 *****************************************************************************/
defcode("!", store, 0, &bnot){
    *((val_t*)*(ArgStack)) = *(ArgStack-1);
    ArgStack -= 2;
}

defcode("@", fetch, 0, &store){
    *(ArgStack) = *((val_t*)*(ArgStack));
}

defcode("+!", addstore, 0, &fetch){
    *((val_t*)*(ArgStack)) += *(ArgStack-1);
    ArgStack -= 2;
}

defcode("-!", substore, 0, &addstore){
    *((val_t*)*(ArgStack)) -= *(ArgStack-1);
    ArgStack -= 2;
}

defcode("b!", bytestore, 0, &substore){
    *((char*)*(ArgStack)) = (char)*(ArgStack-1);
    ArgStack -= 2;
}

defcode("b@", bytefetch, 0, &bytestore){
    *(ArgStack) = *((char*)*(ArgStack));
}

defcode("b@b!", bytecopy, 0, &bytefetch){
}

defcode("bmove", bytemove, 0, &bytecopy){
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    /* Compile-time Assertions */
    CT_ASSERT(sizeof(val_t) == sizeof(val_t*));
    CT_ASSERT(sizeof(val_t) == sizeof(flags_t));

    /* Platform specific initialization */
    latest_val = (val_t)&bytemove;

    /* Start the interpreter */
    EXEC(quit);
    return 0;
}

/* Debugging Words
 *****************************************************************************/
#if 0
defcode("printw", printw, 0, &mem_free){
    word_t* word = (word_t*)*(ArgStack);
    val_t* bytecode = word->code;
    ArgStack--;

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
#endif

