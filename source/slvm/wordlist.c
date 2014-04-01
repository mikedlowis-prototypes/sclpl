#include "wordlist.h"

extern long* ArgStackPtr;

extern void exec_word_def(long const* code);

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,flags,prev) \
    extern void c_name##_code(long const* code);  \
    extern char const c_name##_str[];       \
    word_t const c_name = {                 \
        prev,                               \
        flags,                              \
        c_name##_str,                       \
        &c_name##_code,                     \
        0                                   \
    };                                      \
    char const c_name##_str[] = name_str;   \
    void c_name##_code(long const* inst_ptr)      \

/**
 * Define a built-in word that is defined by references to other words. */
#define defword(name_str,c_name,flags,prev) \
    extern long const c_name##_code[];      \
    extern char const c_name##_str[];       \
    word_t const c_name = {                 \
        prev,                               \
        flags,                              \
        c_name##_str,                       \
        &exec_word_def,                     \
        c_name##_code                       \
    };                                      \
    char const c_name##_str[] = name_str;   \
    long const c_name##_code[] =

#define defvar()

#define defconst()

#define w(name) (long)&name

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

defcode("/", div, 0, &mul){
    *(ArgStackPtr-1) /= *(ArgStackPtr);
    ArgStackPtr--;
}

defcode("%", mod, 0, &div){
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
}

defcode("!", store, 0, &lit){
}

defcode("@", fetch, 0, &store){
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
//defvar("state", , 0, &){
//}
//
//defvar("here", , 0, &){
//}
//
//defvar("latest", , 0, &){
//}
//
//defvar("tos", , 0, &){
//}
//
//defvar("base", , 0, &){
//}

/* Built-in Constants
 *****************************************************************************/
//defconst("version", , 0, &){
//}
//
//defconst("docol", , 0, &){
//}
//
//defconst("f_immed", , 0, &){
//}
//
//defconst("f_hidden", , 0, &){
//}

/* Input/Output Words
 *****************************************************************************/
//defcode("getc", , 0, &){
//}
//
//defcode("putc", , 0, &){
//}
//
//defcode("getw", , 0, &){
//}
//
//defcode("getn", , 0, &){
//}

/* Compiler Words
 *****************************************************************************/
//defcode("findw", , 0, &){
//}
//
//defcode("wcwa", , 0, &){
//}
//
//defcode("wcda", , 0, &){
//}
//
//defcode("create", , 0, &){
//}
//
//defcode(",", , 0, &){
//}
//
//defcode("[", , 0, &){
//}
//
//defcode("]", , 0, &){
//}

//defword(":", , 0, &){
//}
//
//defword(";", , 0, &){
//}

//defcode("immediate", , 0, &){
//}
//
//defcode("hidden", , 0, &){
//}
//
//defcode("hide", , 0, &){
//}
//
//defcode("'", , 0, &){
//}

/* Branching Words
 *****************************************************************************/
//defcode("branch", , 0, &){
//}
//
//defcode("0branch", , 0, &){
//}

/* String Literal Words
 *****************************************************************************/
//defcode("litstring", , 0, &){
//}
//
//defcode("puts", , 0, &){
//}

/* Interpreter Words
 *****************************************************************************/
//defcode("quit", , 0, &){
//}
//
//defcode("interpret", , 0, &){
//}
//
//defcode("char", , 0, &){
//}
//
//defcode("execute", , 0, &){
//}

/* Latest Defined Word
 *****************************************************************************/
word_t const* LatestWord = &bytemove;

