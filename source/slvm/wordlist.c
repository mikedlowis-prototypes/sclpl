#include "wordlist.h"

extern long* ArgStackPtr;

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,flags,prev) \
    extern void c_name##_code(void);   \
    extern char c_name##_str[];        \
    word_t c_name = {                  \
        prev,                          \
        flags,                         \
        c_name##_str,                  \
        &c_name##_code,                \
        0                              \
    };                                 \
    char c_name##_str[] = name_str;    \
    void c_name##_code(void)           \

/**
 * Define a built-in word that is defined by references to other words. */
#define defword(name_str,c_name,flags,prev) \
    extern long c_name##_code[];       \
    extern char c_name##_str[];        \
    word_t c_name = {                  \
        prev,                          \
        flags,                         \
        c_name##_str,                  \
        &do_colon,                     \
        c_name##_code                  \
    };                                 \
    char c_name##_str[] = name_str;    \
    long c_name##_code[] =

/*****************************************************************************/
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

/*****************************************************************************/
word_t* LatestWord = &nrot;

