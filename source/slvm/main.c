#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/**
    This type represents a pointer to a function handler for executing a word.
    For built-in words this typically points to the C function that implements
    the word. For compiled words, this typically points to the docolon function.

    @param code This is a pointer to the next bytecode instruction to execute.
                For built-in words this pointer is 0.
 */
typedef void (*codeword_t)(long* code);

/**
    This structure contains all of the relevant attributes of a word definition
*/
typedef struct word_t {
    /** Pointer to the next most recently defined word in the dictionary. */
    struct word_t const* link;
    /** A collection of flags describing attributes of the word. */
    long flags;
    /** Pointer to the null terminated string that holds the name of the word. */
    char const* name;
    /**
     * Pointer to the execution handler for this word. For words defined in C
     * This points to the implementation function. For words defined in
     * bytecode this will point to the docolon function. */
    codeword_t codeword;
    /**
     * A pointer to the list of instructions that make up this word. For words
     * defined in C this will be 0u (NULL). */
    long* code;
} word_t;

/** Execute a built-in word directly */
#define EXEC(word) (word).codeword((word).code)

/** Pointer to current position on the stack */
static long* ArgStackPtr;

/** Pointer to current position on the stack */
static long* CodePtr;

/** The argument stack */
static long ArgStack[32];

/** A state variable used to flag when the interpreter reads a line of input */
static long Line_Read;

/**
 * This is the "inner" interpreter. This function is responsible for running
 * the threaded code that make up colon defintions. */
static void docolon(long* code) {
    long* prev_code = CodePtr;
    CodePtr = code;
    while(*CodePtr)
    {
        EXEC( *((word_t*)(*CodePtr)) );
        CodePtr++;
    }
    CodePtr = prev_code;
}

static void check_stack(void)
{
    if(ArgStackPtr < (ArgStack-1))
    {
        puts("Stack Underflow!");
        exit(1);
    }

    if(ArgStackPtr > (ArgStack+30))
    {
        puts("Stack Overflow!");
        exit(1);
    }
}

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,flags,prev)      \
    static void c_name##_code(long* code);       \
    static char const c_name##_str[];            \
    static word_t const c_name = {               \
        prev,                                    \
        flags,                                   \
        c_name##_str,                            \
        &c_name##_code,                          \
        0                                        \
    };                                           \
    static char const c_name##_str[] = name_str; \
    static void c_name##_code(long* inst_ptr)    \

/**
 * Define a built-in word representing a variable with the provided initial value */
#define defvar(name_str,c_name,flags,prev,initial) \
    static long c_name##_val = initial;            \
    defcode(name_str,c_name,flags,prev) {          \
        ArgStackPtr++;                             \
        *(ArgStackPtr) = (long)&(c_name##_val);    \
    }

/**
 * Define a built-in word representing a constant with the provided value */
#define defconst(name_str,c_name,flags,prev,value) \
    static long const c_name##_val = value;        \
    defcode(name_str,c_name,flags,prev) {          \
        ArgStackPtr++;                             \
        *(ArgStackPtr) = c_name##_val;             \
    }

/* Built-in Constants
 *****************************************************************************/
defconst("VERSION",  version,  0, 0,        1);
defconst("EXECDEF",  execdef,  0, &version, (long)&docolon);
defconst("F_IMMED",  f_immed,  0, &execdef, 0x01);
defconst("F_HIDDEN", f_hidden, 0, &f_immed, 0x02);

/* Built-in Variables
 *****************************************************************************/
defvar("state",  state,  0, &f_hidden, 0);
defvar("here",   here,   0, &state,    0);
defvar("latest", latest, 0, &here,     0);

/* Input Words
 *****************************************************************************/
defcode("getc", get_char, 0, &latest){
    ArgStackPtr++;
    *(ArgStackPtr) = getc(stdin);
}

defcode("ws?", is_ws, 0, &get_char){
    char ch = *(ArgStackPtr);
    *(ArgStackPtr) = ((ch == ' ')  || (ch == '\t') ||
                      (ch == '\r') || (ch == '\n'));

    /* Note: total hack to get the prompt to reappear when the user hits
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
    *(ArgStackPtr) = (long)&buffer;
}

defcode("findw", find_word, 0, &get_word){
    word_t const* curr = (word_t const*)latest_val;
    char* name = (char*)*(ArgStackPtr);
    while(curr)
    {
        if (!(curr->flags & 0x02) && (0 == strcmp(curr->name,name)))
        {
            break;
        }
        curr = curr->link;
    }
    *(ArgStackPtr) = (long)curr;
}

/* Branching and Literal Words
 *****************************************************************************/
defcode("lit", literal, 0, &find_word){
    CodePtr++;
    ArgStackPtr++;
    *(ArgStackPtr) = *CodePtr;
}

defcode("br", branch, 0, &literal){
  CodePtr++;
  CodePtr += *(CodePtr);
}

defcode("brif", branch_if, 0, &branch){
    if (*ArgStackPtr)
    {
        CodePtr++;
        CodePtr += *(CodePtr);
    }
}

/* Compiler Words
 *****************************************************************************/
defcode("[", lbrack, 0, &branch_if){
    state_val = 1;
}

defcode("]", rbrack, 0x01, &lbrack){
    state_val = 0;
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
    word->flags    = f_hidden_val;
    word->name     = name;
    word->codeword = &docolon;
    word->code     = (long*)malloc(sizeof(long));
    word->code[0]  = 0;
    /* Update Latest and Return the new word */
    latest_val     = (long)word;
    here_val       = (long)word->code;
    *(ArgStackPtr) = (long)word;
}

defcode(",", comma, 0, &create){
    /* Get the word we are currently compiling */
    word_t* word  = (word_t*)latest_val;
    /* Put the next instruction in place of the terminating NULL that "here"
     * points too */
    *((long*)here_val) = *(ArgStackPtr);
    ArgStackPtr--;
    /* Resize the code section and relocate if necessary */
    long currsize = sizeof(long) + (here_val - (long)word->code);
    word->code    = (long*)realloc(word->code, currsize + sizeof(long));
    /* Update "here" and null terminate the code section */
    here_val      = (long)&(word->code[ (currsize / sizeof(long)) ]);
    *((long*)here_val) = 0;
}

defcode("hidden", hidden, 0, &comma){
    ((word_t*)*(ArgStackPtr))->flags ^= f_hidden_val;
}

defcode("immediate", immediate, 0, &hidden){
    ((word_t*)*(ArgStackPtr))->flags ^= f_immed_val;
}

defcode(":", colon, 0, &immediate){
    EXEC(get_word);
    EXEC(create);
    EXEC(lbrack);
}

defcode(";", semicolon, 1, &colon){
    EXEC(rbrack);
    EXEC(hidden);
    ArgStackPtr--;
}

/* Interpreter Words
 *****************************************************************************/
defcode("execw", exec_word, 0, &semicolon){
    word_t* word = (word_t*)(*ArgStackPtr);
    ArgStackPtr--;
    EXEC( *(word) );
}

defcode("parsenum", parse_num, 0, &exec_word){
    char* end;
    long num = strtol((const char *)*(ArgStackPtr), &end, 10);
    if(end != (char *)*(ArgStackPtr))
    {
        *(ArgStackPtr) = num;
    }
    else
    {
        puts("Parse Error");
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
        if((state_val == 0) || (((word_t*)*ArgStackPtr)->flags & f_immed_val))
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
        *(ArgStackPtr) = (long)curr_word;
        EXEC(parse_num);
        if (state_val == 1)
        {
            ArgStackPtr++;
            *(ArgStackPtr) = (long)&literal;
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
            check_stack();
            long stacksz = ArgStackPtr - ArgStack + 1;
            if (stacksz > 5)
                printf("( ... ");
            else
                printf("( ");

            for(i = (stacksz > 5) ? 4 : stacksz-1; i >= 0; i--)
            {
                printf("%lu ", *(ArgStackPtr-i));
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
    long temp = *(ArgStackPtr);
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
    long temp = *(ArgStackPtr);
    *(ArgStackPtr) = *(ArgStackPtr-1);
    *(ArgStackPtr-1) = *(ArgStackPtr-2);
    *(ArgStackPtr-2) = temp;
}

defcode("-rot", nrot, 0, &rot){
    long temp = *(ArgStackPtr-2);
    *(ArgStackPtr-2) = *(ArgStackPtr-1);
    *(ArgStackPtr-1) = *(ArgStackPtr);
    *(ArgStackPtr) = temp;
}

defcode("2drop", twodrop, 0, &nrot){
    ArgStackPtr -= 2;
}

defcode("2dup", twodup, 0, &twodrop){
    ArgStackPtr += 2;
    *(ArgStackPtr-1) = *(ArgStackPtr-3);
    *(ArgStackPtr) = *(ArgStackPtr-2);
}

defcode("2swap", twoswap, 0, &twodup){
}

defcode("?dup", dup_if, 0, &twoswap){
    if (*ArgStackPtr)
    {
        ArgStackPtr++;
        *(ArgStackPtr) = *(ArgStackPtr-1);
    }
}

/* Arithmetic Words
 *****************************************************************************/
defcode("+", add, 0, &dup_if){
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
    *((long*)*(ArgStackPtr)) = *(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("@", fetch, 0, &store){
    *(ArgStackPtr) = *((long*)*(ArgStackPtr));
}

defcode("+!", addstore, 0, &fetch){
    *((long*)*(ArgStackPtr)) += *(ArgStackPtr-1);
    ArgStackPtr -= 2;
}

defcode("-!", substore, 0, &addstore){
    *((long*)*(ArgStackPtr)) -= *(ArgStackPtr-1);
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

/* Main
 *****************************************************************************/
int main(int argc, char** argv)
{
    ArgStackPtr = ArgStack - 1;
    latest_val = (long)&bytemove;
    EXEC(quit);
    return 0;
}

