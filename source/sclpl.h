#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <setjmp.h>

static void fatal(char* estr) {
    perror(estr);
    exit(1);
}

static void* emalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) fatal("malloc()");
    return ptr;
}

/* Vector Implementation
 *****************************************************************************/
typedef struct {
    size_t count;
    size_t size;
    size_t capacity;
    void** buffer;
} vec_t;

void vec_init(vec_t* vec);
void vec_deinit(vec_t* vec);
void vec_clear(vec_t* vec);
size_t vec_size(vec_t* vec);
void* vec_at(vec_t* vec, size_t index);
void vec_push_back(vec_t* vec, void* data);
void vec_set(vec_t* vec, size_t index, void* data);

/* Token Types
 *****************************************************************************/
typedef enum {
    T_NONE, T_ERROR, T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE,
    T_RBRACE, T_LBRACK, T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE,
    T_END, T_COLON, T_AMP, T_LET, T_IF, T_THEN, T_ELSE, T_END_FILE
} TokType;

typedef struct {
    const char* file;
    size_t line;
    size_t col;
    TokType type;
    union {
        char* text;
        uint32_t character;
        intptr_t integer;
        double floating;
        bool boolean;
    } value;
} Tok;

/* AST Types
 *****************************************************************************/
typedef enum {
    AST_STRING, AST_SYMBOL, AST_CHAR, AST_INT, AST_FLOAT, AST_BOOL, AST_IDENT,
    AST_DEF, AST_IF, AST_FUNC, AST_FNAPP
} ASTType;

typedef struct AST {
    ASTType type;
    union {
        /* Definition Node */
        struct {
            char* name;
            struct AST* type;
            struct AST* value;
        } def;
        /* If Expression */
        struct {
            struct AST* cond;
            struct AST* bthen;
            struct AST* belse;
        } ifexpr;
        /* Function */
        struct {
            vec_t args;
            struct AST* body;
        } func;
        /* Function Application */
        struct {
            struct AST* fn;
            vec_t args;
        } fnapp;
        /* String, Symbol, Identifier */
        char* text;
        /* Character */
        uint32_t character;
        /* Integer */
        intptr_t integer;
        /* Float */
        double floating;
        /* Bool */
        bool boolean;
    } value;
} AST;

/* String */
AST* String(Tok* val);
char* string_value(AST* val);

/* Symbol */
AST* Symbol(Tok* val);
char* symbol_value(AST* val);

/* Character */
AST* Char(Tok* val);
uint32_t char_value(AST* val);

/* Integer */
AST* Integer(Tok* val);
intptr_t integer_value(AST* val);

/* Float */
AST* Float(Tok* val);
double float_value(AST* val);

/* Bool */
AST* Bool(Tok* val);
bool bool_value(AST* val);

/* Ident */
AST* Ident(Tok* val);
char* ident_value(AST* val);

/* Definition */
AST* Def(Tok* name, AST* value);
char* def_name(AST* def);
AST* def_value(AST* def);

/* If Expression */
AST* IfExpr(void);
AST* ifexpr_cond(AST* ifexpr);
void ifexpr_set_cond(AST* ifexpr, AST* cond);
AST* ifexpr_then(AST* ifexpr);
void ifexpr_set_then(AST* ifexpr, AST* bthen);
AST* ifexpr_else(AST* ifexpr);
void ifexpr_set_else(AST* ifexpr, AST* belse);

/* Function */
AST* Func(void);
vec_t* func_args(AST* func);
AST* func_body(AST* func);
void func_add_arg(AST* func, AST* arg);
void func_set_body(AST* func, AST* body);

/* Function Application */
AST* FnApp(AST* fn);
AST* fnapp_fn(AST* fnapp);
void fnapp_set_fn(AST* fnapp, AST* fn);
vec_t* fnapp_args(AST* fnapp);
void fnapp_add_arg(AST* func, AST* arg);

/* Pretty Printing
 *****************************************************************************/
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
void pprint_tree(FILE* file, AST* tree, int depth);

/* Lexer and Parser Types
 *****************************************************************************/
typedef struct {
    FILE* input;
    Tok tok;
} Parser;

void gettoken(Parser* ctx, Tok* tok);
AST* toplevel(Parser* p);

/* Option Parsing
 *****************************************************************************/

/* This variable contains the value of argv[0] so that it can be referenced
 * again once the option parsing is done. This variable must be defined by the
 * program.
 *
 * NOTE: Ensure that you define this variable with external linkage (i.e. not
 * static)
 */
extern char* ARGV0;

/* This is a helper function used by the macros in this file to parse the next
 * option from the command line.
 */
static inline char* __getopt(int* p_argc, char*** p_argv) {
    if (!(*p_argv)[0][1] && !(*p_argv)[1]) {
        return (char*)0;
    } else if ((*p_argv)[0][1]) {
        return &(*p_argv)[0][1];
    } else {
        *p_argv = *p_argv + 1;
        *p_argc = *p_argc - 1;
        return (*p_argv)[0];
    }
}

/* This macro is almost identical to the ARGBEGIN macro from suckless.org. If
 * it ain't broke, don't fix it. */
#define OPTBEGIN                                                              \
    for (                                                                     \
        ARGV0 = *argv, argc--, argv++;                                        \
        argv[0] && argv[0][1] && argv[0][0] == '-';                           \
        argc--, argv++                                                        \
    ) {                                                                       \
        int brk_; char argc_ , **argv_, *optarg_;                             \
        if (argv[0][1] == '-' && !argv[0][2]) {                               \
            argv++, argc--; break;                                            \
        }                                                                     \
        for (brk_=0, argv[0]++, argv_=argv; argv[0][0] && !brk_; argv[0]++) { \
            if (argv_ != argv) break;                                         \
            argc_ = argv[0][0];                                               \
            switch (argc_)

/* Terminate the option parsing. */
#define OPTEND }}

/* Get the current option chracter */
#define OPTC() (argc_)

/* Get an argument from the command line and return it as a string. If no
 * argument is available, this macro returns NULL */
#define OPTARG() \
    (optarg_ = __getopt(&argc,&argv), brk_ = (optarg_!=0), optarg_)

/* Get an argument from the command line and return it as a string. If no
 * argument is available, this macro executes the provided code. If that code
 * returns, then abort is called. */
#define EOPTARG(code) \
    (optarg_ = __getopt(&argc,&argv), \
     (!optarg_ ? ((code), abort(), (char*)0) : (brk_ = 1, optarg_)))

/* Helper macro to recognize number options */
#define OPTNUM \
    case '0':  \
    case '1':  \
    case '2':  \
    case '3':  \
    case '4':  \
    case '5':  \
    case '6':  \
    case '7':  \
    case '8':  \
    case '9'

/* Helper macro to recognize "long" options ala GNU style. */
#define OPTLONG \
    case '-'
