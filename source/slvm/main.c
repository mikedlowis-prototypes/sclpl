#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include "wordlist.h"

typedef void (*codeword_t)(long const*);

typedef struct word_t {
    struct word_t const* link;
    long flags;
    char const* name;
    codeword_t codeword;
    long* code;
} word_t;

static void exec_word_def(long const* code);

static long* ArgStackPtr;

static word_t const* LatestWord;

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,flags,prev)         \
    static void c_name##_code(long const* code);    \
    static char const c_name##_str[];               \
    static word_t const c_name = {                  \
        prev,                                       \
        flags,                                      \
        c_name##_str,                               \
        &c_name##_code,                             \
        0                                           \
    };                                              \
    static char const c_name##_str[] = name_str;    \
    static void c_name##_code(long const* inst_ptr) \

/**
 * Define a built-in word that is defined by references to other words. */
#define defword(name_str,c_name,flags,prev)      \
    static long c_name##_code[];                 \
    static char const c_name##_str[];            \
    static word_t const c_name = {               \
        prev,                                    \
        flags,                                   \
        c_name##_str,                            \
        &exec_word_def,                          \
        c_name##_code                            \
    };                                           \
    static char const c_name##_str[] = name_str; \
    static long c_name##_code[] =

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

#define w(name) (long)&name

#define EXEC(word) word->codeword(word->code)

#define NEXT 0u

/* Built-in Primitive Words
 *****************************************************************************/
defcode("drop", drop, 0, 0){
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

defcode("?dup", qdup, 0, &twoswap){
}

defcode("1+", incr, 0, &qdup){
    *(ArgStackPtr) += 1;
}

defcode("1-", decr, 0, &incr){
    *(ArgStackPtr) -= 1;
}

defcode("4+", incr4, 0, &decr){
    *(ArgStackPtr) += 1;
}

defcode("4-", decr4, 0, &incr4){
    *(ArgStackPtr) -= 1;
}

defcode("+", add, 0, &decr4){
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

defcode("/%", divmod, 0, &mod){
}

defcode("=", equal, 0, &divmod){
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

defcode("0=", zeroeq, 0, &greaterthaneq){
    *(ArgStackPtr) = *(ArgStackPtr) == 0;
}

defcode("0!=", zeroneq, 0, &zeroeq){
    *(ArgStackPtr) = *(ArgStackPtr) != 0;
}

defcode("0<", zerolt, 0, &zeroneq){
    *(ArgStackPtr) = *(ArgStackPtr) < 0;
}

defcode("0>", zerogt, 0, &zerolt){
    *(ArgStackPtr) = *(ArgStackPtr) > 0;
}

defcode("0<=", zerolte, 0, &zerogt){
    *(ArgStackPtr) = *(ArgStackPtr) <= 0;
}

defcode("0>=", zerogte, 0, &zerolte){
    *(ArgStackPtr) = *(ArgStackPtr) >= 0;
}

defcode("and", and, 0, &zerogte){
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

defcode("lit", lit, 0, &bnot){
    ArgStackPtr++;
    *(ArgStackPtr) = *(inst_ptr);
}

defcode("!", store, 0, &lit){
}

defcode("@", fetch, 0, &store){
    puts("@ Executed");
}

defcode("+!", addstore, 0, &fetch){
}

defcode("-!", substore, 0, &addstore){
}

defcode("b!", bytestore, 0, &substore){
}

defcode("b@", bytefetch, 0, &bytestore){
}

defcode("b@b!", bytecopy, 0, &bytefetch){
}

defcode("bmove", bytemove, 0, &bytecopy){
}

/* Built-in Variables
 *****************************************************************************/
defvar("state", state, 0, &bytemove, 0);
defvar("here", here, 0, &state, 0);
defvar("latest", latest, 0, &here, 0);
defvar("tos", tos, 0, &latest, 0);
defvar("base", base, 0, &tos, 0);

/* Built-in Constants
 *****************************************************************************/
defconst("VERSION", version, 0, &base, 1);
defconst("EXECDEF", execdef, 0, &version, (long)&exec_word_def);
defconst("F_IMMED", f_immed, 0, &execdef, 1);
defconst("F_HIDDEN", f_hidden, 0, &f_immed, 2);

/* Input/Output Words
 *****************************************************************************/
int is_whitespace(char ch)
{
    return ((ch == ' ')  || (ch == '\t') || (ch == '\r') || (ch == '\n'));
}

defcode("getc", get_io_c, 0, &f_hidden){
    ArgStackPtr++;
    *(ArgStackPtr) = getc(stdin);
}

defcode("putc", put_io_c, 0, &get_io_c){
    putc((char)*(ArgStackPtr), stdout);
    ArgStackPtr--;
}

defcode("getw", parse_word, 0, &put_io_c){
    static char buffer[32];
    int i = 0;
    /* Skip any whitespace */
    do {
        buffer[i] = getc(stdin);
    } while(is_whitespace(buffer[i++]));

    /* Read the rest of the word */
    while(!is_whitespace(buffer[i] = getc(stdin)) && (i < 31))
    {
        i++;
    }

    /* Terminate the string */
    buffer[i] = '\0';

    /* Return the word */
    ArgStackPtr++;
    *(ArgStackPtr) = (long)&buffer;
}

defcode("getn", getn, 0, &parse_word){
    long number = 0;
    *(ArgStackPtr) = number;
}

/* Compiler Words
 *****************************************************************************/
defcode("findw", findw, 0, &getn){
    puts("finding word");
    word_t const* curr = (word_t const*)latest_val;
    char* name = (char*)*(ArgStackPtr);
    while(curr)
    {
        if (0 == strcmp(curr->name,name))
        {
            break;
        }
        curr = curr->link;
    }
    *(ArgStackPtr) = (long)curr;
}

defcode("wcwa", code_word_addr, 0, &findw){
    word_t const* word = (word_t const*)*(ArgStackPtr);
    *(ArgStackPtr) = (long)word->codeword;
}

defcode("wcda", code_data_addr, 0, &code_word_addr){
    word_t const* word = (word_t const*)*(ArgStackPtr);
    *(ArgStackPtr) = (long)word->code;
}

defcode("create", create, 0, &code_data_addr){
    /* Copy the name string */
    size_t namesz = strlen((char*)*(ArgStackPtr));
    char* name = (char*)malloc( namesz );
    strcpy(name, (char*)*(ArgStackPtr));
    /* Create the word entry */
    word_t* word   = (word_t*)malloc(sizeof(word_t));
    word->link     = LatestWord;
    word->flags    = f_hidden_val;
    word->name     = name;
    word->codeword = exec_word_def;
    word->code     = (long*)malloc(sizeof(long));
    word->code[0]  = 0;
    /* Update Latest and Return the new word */
    latest_val     = (long)word;
    here_val       = (long)word->code;
    *(ArgStackPtr) = (long)word;
}

defcode("immediate", immediate, 0, &create){
    ((word_t*)latest_val)->flags ^= f_immed_val;
}

defcode("hidden", hidden, 0, &immediate){
    ((word_t*)latest_val)->flags ^= f_hidden_val;
}

defcode(",", comma, 0, &hidden){
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

defcode("[", lbrack, 0, &comma){
    state_val = 0;
}

defcode("]", rbrack, 0, &lbrack){
    state_val = 1;
}

defword(":", colon, 0, &rbrack){
    w(parse_word),
    w(create),
    w(rbrack),
    NEXT
};

defcode(";", semicolon, 0, &colon){
    ((word_t*)latest_val)->flags ^= f_hidden_val;
    state_val = 0;
}

/* Branching Words
 *****************************************************************************/
defcode("branch", branch, 0, &semicolon){
}

defcode("0branch", branch_if_0, 0, &branch){
}

/* Interpreter Words
 *****************************************************************************/
defcode("execute", execute, 0, &branch_if_0){
    word_t* word = (word_t*)*(ArgStackPtr);
    ArgStackPtr--;
    word->codeword(word->code);
}

defcode("interpret", interpret, 0, &execute){
    printf("=> ");
    /* Parse a word */
    parse_word.codeword(parse_word.code);
    /* Find the word */
    findw.codeword(findw.code);
    /* Execute the word */
    if (*ArgStackPtr) {
        execute.codeword(execute.code);
    /* or parse it as a number */
    } else {
        //number.codeword(number.code);
    }
}

defcode("quit", quit, 0, &interpret){
    puts("System Reset");
    latest_val = (long)LatestWord;
    while(1)
    {
        interpret.codeword(interpret.code);
    }
}

/* Latest Defined Word
 *****************************************************************************/

int main(int argc, char** argv)
{
    long stack[32] = {0};
    ArgStackPtr = stack;
    LatestWord = &quit;
    LatestWord->codeword(LatestWord->code);
    return 0;
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

