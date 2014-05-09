/**
  @file slvm.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef SLVM_H
#define SLVM_H

#include <stdint.h>

/* Choose a width for val_t that matches the pointer size of the target
 * architecture. Defaults to simply a long but can be overridden for specific
 * cases */
#if defined(_16BIT_)
    typedef int16_t val_t;
#elif defined(_32BIT_)
    typedef int32_t val_t;
#elif defined(_64BIT_)
    typedef int64_t val_t;
#else
    /* hope for the best? */
    typedef long val_t;
#endif

/**
    The number of bits of the flags field dedicated to holding the size of the
    bytecode vector.
*/
#define CODE_SZ_BITS ((sizeof(val_t) * 8u) - 8u)

/**
    This type represents a pointer to a function handler for executing a word.
    For built-in words this typically points to the C function that implements
    the word. For compiled words, this typically points to the docolon function.

    @param code This is a pointer to the next bytecode instruction to execute.
                For built-in words this pointer is 0.
 */
typedef void (*codeword_t)(val_t* code);

/** A collection of flags describing attributes of the word. */
typedef union {
    /** The value of all flag bits combined */
    val_t value;
    /** Accessors for individual flag fields */
    struct {
        /** Flag if this word should be hidden from the interpreter */
        val_t hidden   : 1;
        /** flag if this word should be executed at compile time */
        val_t immed    : 1;
        /** Pads the flags to 8-bits */
        val_t padding  : 6;
        /** The length of the bytecode section of the word */
        val_t codesize : CODE_SZ_BITS;
    } attr;
} flags_t;

/**
    This structure contains all of the relevant attributes of a word definition
*/
typedef struct word_t {
    /** Pointer to the next most recently defined word in the dictionary. */
    struct word_t const* link;
    /** A collection of flags describing attributes of the word. */
    flags_t flags;
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
    val_t* code;
} word_t;

/** This structure defines a dictionary of defined words. */
typedef struct dict_t {
    /** Pointer to the previously loaded dictionary */
    struct dict_t* p_prev;
    /** The name of the dictionary */
    char* name;
    /** Pointer to the most recently defined word in this dictionary */
    word_t* p_words;
} dict_t;

/** Execute a built-in word directly */
#define EXEC(word) (word).codeword((word).code)

/** The maximum number of entries that can be on the argument stack */
#ifndef STACK_SIZE
#define STACK_SIZE 32
#endif

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,immed,prev) \
    void c_name##_code(val_t* code);        \
    word_t const c_name = {                 \
        prev,                               \
        {.attr = { 0, immed, 0, 0 }},       \
        name_str,                           \
        &c_name##_code,                     \
        0                                   \
    };                                      \
    void c_name##_code(val_t* inst_ptr)     \

/**
 * Define a built-in word representing a variable with the provided initial value */
#define defvar(name_str,c_name,immed,prev,initial) \
    val_t c_name##_val = initial;                  \
    defcode(name_str,c_name,immed,prev) {          \
        ArgStack++;                                \
        *(ArgStack) = (val_t)&(c_name##_val);      \
    }

/**
 * Define a built-in word representing a constant with the provided value */
#define defconst(name_str,c_name,immed,prev,value) \
    val_t const c_name##_val = value;              \
    defcode(name_str,c_name,immed,prev) {          \
        ArgStack++;                                \
        *(ArgStack) = c_name##_val;                \
    }

/**
 * A little C macro hack that allows for some compile time assertions. When the
 * expression evaluates to false (0) at compile time, the expanded switch
 * statement will contain two case statements for the value 0 which is a compile
 * error. Thus the build will break alerting the user that something went
 * horribly wrong. */
#define CT_ASSERT(expr) \
    switch(1){ case(0):break; case(expr):break; default: break; }

/**
 * This is the "inner" interpreter. This function is responsible for running
 * the threaded code that make up colon defintions.
 *
 * @param code This is a pointer to the next instruction to be executed.
 * */
void docolon(val_t* code);

void slvm_init(void);

#endif /* SLVM_H */
