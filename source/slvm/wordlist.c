#include "wordlist.h"

extern long* ArgStackPtr;

extern void exec_word_def(long* code);

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,flags,prev) \
    extern void c_name##_code(long* code);  \
    extern char c_name##_str[];             \
    word_t c_name = {                       \
        prev,                               \
        flags,                              \
        c_name##_str,                       \
        &c_name##_code,                     \
        0                                   \
    };                                      \
    char c_name##_str[] = name_str;         \
    void c_name##_code(long* inst_ptr)      \

/**
 * Define a built-in word that is defined by references to other words. */
#define defword(name_str,c_name,flags,prev) \
    extern long c_name##_code[];       \
    extern char c_name##_str[];        \
    word_t c_name = {                  \
        prev,                          \
        flags,                         \
        c_name##_str,                  \
        &exec_word_def,                \
        c_name##_code                  \
    };                                 \
    char c_name##_str[] = name_str;    \
    long c_name##_code[] =

#define w(name) (long)&name

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

defcode("2drop", twodrop, 0, &nrot){}

defcode("2dup", twodup, 0, &twodrop){}

defcode("2swap", twoswap, 0, &twodup){}

defcode("?dup", qdup, 0, &twoswap){}

defcode("1+", incr, 0, &qdup){}

defcode("1-", decr, 0, &incr){}

defcode("4+", incr4, 0, &decr){}

defcode("4-", decr4, 0, &incr4){}

defcode("+", add, 0, &decr4){}

defcode("-", sub, 0, &add){}

defcode("*", mul, 0, &sub){}

defcode("/", div, 0, &mul){}

defcode("/mod", divmod, 0, &div){}

defcode("=", equal, 0, &divmod){}

defcode("!=", notequal, 0, &equal){}

defcode("<", lessthan, 0, &notequal){}

defcode(">", greaterthan, 0, &lessthan){}

defcode("<=", lessthaneq, 0, &greaterthan){}

defcode(">=", greaterthaneq, 0, &lessthaneq){}

defcode("0=", zeroeq, 0, &greaterthaneq){}

defcode("0!=", zeroneq, 0, &zeroeq){}

defcode("0<", zerolt, 0, &zeroneq){}

defcode("0>", zerogt, 0, &zerolt){}

defcode("0<=", zerolte, 0, &zerogt){}

defcode("0>=", zerogte, 0, &zerolte){}

defcode("and", and, 0, &zerogte){}

defcode("or", or, 0, &and){}

defcode("not", not, 0, &or){}

defcode("band", band, 0, &not){}

defcode("bor", bor, 0, &band){}

defcode("bxor", bxor, 0, &bor){}

defcode("bnot", bnot, 0, &bxor){}

defcode("lit", lit, 0, &bnot){}

defcode("!", store, 0, &lit){}

defcode("@", fetch, 0, &store){}

defcode("+!", addstore, 0, &fetch){}

defcode("-!", substore, 0, &addstore){}

defcode("b!", bytestore, 0, &substore){}

defcode("b@", bytefetch, 0, &bytestore){}

defcode("b@b!", bytecopy, 0, &bytefetch){}

defcode("bmove", bytemove, 0, &bytecopy){}

/* Built-in Variables
 *****************************************************************************/

/* Built-in Constants
 *****************************************************************************/

/* Built-in Defined Words
 *****************************************************************************/

/* Latest Defined Word
 *****************************************************************************/
word_t* LatestWord = &bytemove;

