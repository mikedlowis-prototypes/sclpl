/**
  @file slvm.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef SLVM_H
#define SLVM_H

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
    struct {
        long f_hidden : 1;  /*< Flag if this word should be hidden from the interpreter */
        long f_immed  : 1;  /*< flag if this word should be executed at compile time */
        long padding  : 6;  /*< Pads the flags to 8-bits */
        long codesize : 24; /*< The lenght of the bytecode section of the word */
    } flags;
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

/** The maximum number of entries that can be on the argument stack */
#ifndef ARG_STACK_SIZE
#define ARG_STACK_SIZE 32
#endif

/**
 * Define a built-in word that executes native code */
#define defcode(name_str,c_name,immed,prev)   \
    static void c_name##_code(long* code);    \
    static word_t const c_name = {            \
        prev,                                 \
        { 0, immed, 0, 0 },                   \
        name_str,                             \
        &c_name##_code,                       \
        0                                     \
    };                                        \
    static void c_name##_code(long* inst_ptr) \

/**
 * Define a built-in word representing a variable with the provided initial value */
#define defvar(name_str,c_name,immed,prev,initial) \
    static long c_name##_val = initial;            \
    defcode(name_str,c_name,immed,prev) {          \
        ArgStackPtr++;                             \
        *(ArgStackPtr) = (long)&(c_name##_val);    \
    }

/**
 * Define a built-in word representing a constant with the provided value */
#define defconst(name_str,c_name,immed,prev,value) \
    static long const c_name##_val = value;        \
    defcode(name_str,c_name,immed,prev) {          \
        ArgStackPtr++;                             \
        *(ArgStackPtr) = c_name##_val;             \
    }

/**
 * This is the "inner" interpreter. This function is responsible for running
 * the threaded code that make up colon defintions.
 *
 * @param code This is a pointer to the next instruction to be executed.
 * */
void docolon(long* code);

#endif /* SLVM_H */
