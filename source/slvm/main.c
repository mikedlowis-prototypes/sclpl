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

/** The argument stack */
static long ArgStack[32];

/**
 * This is the "inner" interpreter. This function is responsible for running
 * the threaded code that make up colon defintions. */
static void docolon(long* code) {
    while(*code)
    {
        if(*code == LONG_MAX)
        {
            code++;
            ArgStackPtr++;
            *(ArgStackPtr) = *code;
        }
        else
        {
            EXEC( *((word_t*)(*code)) );
        }
        code++;
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
        if(wschar == '\n')
            printf((state_val) ? "=> " : ".. ");
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

/* Compiler Words
 *****************************************************************************/
defcode("[", lbrack, 0, &find_word){
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

defcode("hide", hide, 0, &comma){
    ((word_t*)*(ArgStackPtr))->flags |= 0x02;
}

defcode("unhide", unhide, 0, &hide){
    ((word_t*)*(ArgStackPtr))->flags ^= 0x02;
}

defcode(":", colon, 0, &unhide){
    EXEC(get_word);
    EXEC(create);
    EXEC(lbrack);
}

defcode(";", semicolon, 1, &colon){
    EXEC(rbrack);
    EXEC(unhide);
}

/* Interpreter Words
 *****************************************************************************/
defcode("execw", exec_word, 0, &semicolon){
    word_t* word = (word_t*)(*ArgStackPtr);
    ArgStackPtr--;
    EXEC( *(word) );
}

defcode("parsenum", parse_num, 0, &exec_word){
    *(ArgStackPtr) = strtol((const char *)*(ArgStackPtr), NULL, 10);
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
            *(ArgStackPtr) = LONG_MAX;
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
    puts("System Reset");
    while(1)
    {
        EXEC(interpret);

        printf("%d - %lu %s\n", 0, ArgStack[0], &ArgStack[0] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 1, ArgStack[1], &ArgStack[1] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 2, ArgStack[2], &ArgStack[2] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 3, ArgStack[3], &ArgStack[3] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 4, ArgStack[4], &ArgStack[4] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 5, ArgStack[5], &ArgStack[5] == ArgStackPtr ? "<-" : "");
        printf("%d - %lu %s\n", 6, ArgStack[6], &ArgStack[6] == ArgStackPtr ? "<-" : "");
    }
}

/* Main
 *****************************************************************************/
int main(int argc, char** argv)
{
    ArgStack[0] = 1111;
    ArgStack[1] = 2222;
    ArgStack[2] = 3333;
    ArgStack[3] = 4444;
    ArgStackPtr = ArgStack;
    latest_val = (long)&quit;
    EXEC(quit);

    return 0;
}



//
//defcode("wcwa", code_word_addr, 0, &put_io_c){
//    word_t const* word = (word_t const*)*(ArgStackPtr);
//    *(ArgStackPtr) = (long)word->codeword;
//}
//
//defcode("wcda", code_data_addr, 0, &code_word_addr){
//    word_t const* word = (word_t const*)*(ArgStackPtr);
//    *(ArgStackPtr) = (long)word->code;
//}
//
//defcode("immediate", immediate, 0, &code_data_addr){
//    ((word_t*)latest_val)->flags ^= f_immed_val;
//}
//
//defcode("hidden", hidden, 0, &immediate){
//    ((word_t*)latest_val)->flags ^= f_hidden_val;
//}
//

/* Branching Words
 *****************************************************************************/
//defcode("branch", branch, 0, &semicolon){
//}
//
//defcode("0branch", branch_if_0, 0, &branch){
//}

/* Built-in Primitive Words
 *****************************************************************************/
//defcode("drop", drop, 0, &create){
//    ArgStackPtr--;
//}
//
//defcode("swap", swap, 0, &drop){
//    long temp = *(ArgStackPtr);
//    *(ArgStackPtr) = *(ArgStackPtr-1);
//    *(ArgStackPtr-1) = temp;
//}
//
//defcode("dup", dup, 0, &swap){
//    ArgStackPtr++;
//    *(ArgStackPtr) = *(ArgStackPtr-1);
//}
//
//defcode("over", over, 0, &dup){
//    ArgStackPtr++;
//    *(ArgStackPtr) = *(ArgStackPtr-2);
//}
//
//defcode("rot", rot, 0, &over){
//    long temp = *(ArgStackPtr);
//    *(ArgStackPtr) = *(ArgStackPtr-1);
//    *(ArgStackPtr-1) = *(ArgStackPtr-2);
//    *(ArgStackPtr-2) = temp;
//}
//
//defcode("-rot", nrot, 0, &rot){
//    long temp = *(ArgStackPtr-2);
//    *(ArgStackPtr-2) = *(ArgStackPtr-1);
//    *(ArgStackPtr-1) = *(ArgStackPtr);
//    *(ArgStackPtr) = temp;
//}
//
//defcode("2drop", twodrop, 0, &nrot){
//    ArgStackPtr -= 2;
//}
//
//defcode("2dup", twodup, 0, &twodrop){
//    ArgStackPtr += 2;
//    *(ArgStackPtr-1) = *(ArgStackPtr-3);
//    *(ArgStackPtr) = *(ArgStackPtr-2);
//}
//
//defcode("2swap", twoswap, 0, &twodup){
//}
//
//defcode("?dup", qdup, 0, &twoswap){
//}
//
//defcode("1+", incr, 0, &qdup){
//    *(ArgStackPtr) += 1;
//}
//
//defcode("1-", decr, 0, &incr){
//    *(ArgStackPtr) -= 1;
//}
//
//defcode("4+", incr4, 0, &decr){
//    *(ArgStackPtr) += 1;
//}
//
//defcode("4-", decr4, 0, &incr4){
//    *(ArgStackPtr) -= 1;
//}
//
//defcode("+", add, 0, &create){
//    *(ArgStackPtr-1) += *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("-", sub, 0, &add){
//    *(ArgStackPtr-1) -= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("*", mul, 0, &sub){
//    *(ArgStackPtr-1) *= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("/", divide, 0, &mul){
//    *(ArgStackPtr-1) /= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("%", mod, 0, &divide){
//    *(ArgStackPtr-1) %= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("/%", divmod, 0, &mod){
//}
//
//defcode("=", equal, 0, &divmod){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) == *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("!=", notequal, 0, &equal){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) != *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("<", lessthan, 0, &notequal){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) < *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode(">", greaterthan, 0, &lessthan){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) > *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("<=", lessthaneq, 0, &greaterthan){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) <= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode(">=", greaterthaneq, 0, &lessthaneq){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) >= *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("0=", zeroeq, 0, &greaterthaneq){
//    *(ArgStackPtr) = *(ArgStackPtr) == 0;
//}
//
//defcode("0!=", zeroneq, 0, &zeroeq){
//    *(ArgStackPtr) = *(ArgStackPtr) != 0;
//}
//
//defcode("0<", zerolt, 0, &zeroneq){
//    *(ArgStackPtr) = *(ArgStackPtr) < 0;
//}
//
//defcode("0>", zerogt, 0, &zerolt){
//    *(ArgStackPtr) = *(ArgStackPtr) > 0;
//}
//
//defcode("0<=", zerolte, 0, &zerogt){
//    *(ArgStackPtr) = *(ArgStackPtr) <= 0;
//}
//
//defcode("0>=", zerogte, 0, &zerolte){
//    *(ArgStackPtr) = *(ArgStackPtr) >= 0;
//}
//
//defcode("and", and, 0, &zerogte){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) && *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("or", or, 0, &and){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) || *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("not", not, 0, &or){
//    *(ArgStackPtr) = !(*(ArgStackPtr));
//}
//
//defcode("band", band, 0, &not){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) & *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("bor", bor, 0, &band){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) | *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("bxor", bxor, 0, &bor){
//    *(ArgStackPtr-1) = *(ArgStackPtr-1) ^ *(ArgStackPtr);
//    ArgStackPtr--;
//}
//
//defcode("bnot", bnot, 0, &bxor){
//    *(ArgStackPtr) = ~(*(ArgStackPtr));
//}
//
//defcode("lit", lit, 0, &bnot){
//    ArgStackPtr++;
//    *(ArgStackPtr) = *(inst_ptr);
//}
//
//defcode("!", store, 0, &lit){
//}
//
//defcode("@", fetch, 0, &store){
//    puts("@ Executed");
//}
//
//defcode("+!", addstore, 0, &fetch){
//}
//
//defcode("-!", substore, 0, &addstore){
//}
//
//defcode("b!", bytestore, 0, &substore){
//}
//
//defcode("b@", bytefetch, 0, &bytestore){
//}
//
//defcode("b@b!", bytecopy, 0, &bytefetch){
//}
//
//defcode("bmove", bytemove, 0, &bytecopy){
//}

/* Input/Output Words
 *****************************************************************************/
//defcode("putc", put_io_c, 0, &bytemove){
//    putc((char)*(ArgStackPtr), stdout);
//    ArgStackPtr--;
//}

/* Compiler Words
 *****************************************************************************/
//defcode("wcwa", code_word_addr, 0, &put_io_c){
//    word_t const* word = (word_t const*)*(ArgStackPtr);
//    *(ArgStackPtr) = (long)word->codeword;
//}
//
//defcode("wcda", code_data_addr, 0, &code_word_addr){
//    word_t const* word = (word_t const*)*(ArgStackPtr);
//    *(ArgStackPtr) = (long)word->code;
//}
//
//defcode("immediate", immediate, 0, &code_data_addr){
//    ((word_t*)latest_val)->flags ^= f_immed_val;
//}
//
//defcode("hidden", hidden, 0, &immediate){
//    ((word_t*)latest_val)->flags ^= f_hidden_val;
//}
//
//defcode(",", comma, 0, &hidden){
//    /* Get the word we are currently compiling */
//    word_t* word  = (word_t*)latest_val;
//    /* Put the next instruction in place of the terminating NULL that "here"
//     * points too */
//    *((long*)here_val) = *(ArgStackPtr);
//    ArgStackPtr--;
//    /* Resize the code section and relocate if necessary */
//    long currsize = sizeof(long) + (here_val - (long)word->code);
//    word->code    = (long*)realloc(word->code, currsize + sizeof(long));
//    /* Update "here" and null terminate the code section */
//    here_val      = (long)&(word->code[ (currsize / sizeof(long)) ]);
//    *((long*)here_val) = 0;
//}
//
//defcode("[", lbrack, 0, &comma){
//    state_val = 1;
//}
//
//defcode("]", rbrack, 0, &lbrack){
//    state_val = 0;
//}
//
//defcode(":", colon, 0, &rbrack){
//    EXEC(get_word);
//    EXEC(create);
//    EXEC(rbrack);
//}
//
//defcode(";", semicolon, 0, &colon){
//    ((word_t*)latest_val)->flags ^= f_hidden_val;
//    state_val = 0;
//}



