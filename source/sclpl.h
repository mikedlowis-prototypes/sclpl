/**
  @file sclpl.h
*/
#ifndef SCLPL_H
#define SCLPL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <setjmp.h>
#include <opt.h>

/* Garbage Collection
 *****************************************************************************/
typedef void (*destructor_t)(void*);

void gc_init(void** stack_bottom);
void gc_deinit(void);
void gc_collect(void);
void* gc_alloc(size_t size, destructor_t destructor);
void* gc_addref(void* ptr);
void gc_delref(void* ptr);
void gc_swapref(void** dest, void* newref);

// Redefine main
extern int user_main(int argc, char** argv);

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
    T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE, T_RBRACE, T_LBRACK,
    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END, T_COLON, T_AMP,
    T_REQUIRE, T_DEF, T_IF, T_FN, T_THEN, T_ELSE, T_END_FILE
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
typedef enum ASTType {
    AST_STRING, AST_SYMBOL, AST_CHAR, AST_INT, AST_FLOAT, AST_BOOL, AST_IDENT,
    AST_REQ, AST_DEF, AST_IF, AST_FUNC, AST_FNAPP, AST_LET, AST_TEMP
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
        /* Let Expression */
        struct {
            struct AST* temp;
            struct AST* value;
            struct AST* body;
        } let;
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

/* Temp Variable */
AST* TempVar(void);
intptr_t temp_value(AST* val);

/* Require */
AST* Require(Tok* name);
char* require_name(AST* req);

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

/* Let Expression */
AST* Let(AST* temp, AST* val, AST* body);
AST* let_var(AST* let);
AST* let_val(AST* let);
AST* let_body(AST* let);
void let_set_body(AST* let, AST* body);

/* Symbol Table
 *****************************************************************************/
typedef struct SymTable {
    struct SymTable* next;
    char* name;
} SymTable;

SymTable* symbol_new(void);
//void symbol_free(SymTable* sym);
SymTable* symbol_push(SymTable* top, SymTable* newtop);
SymTable* symbol_pop(SymTable* top);
SymTable* symbol_get(const char* name);
SymTable* symbol_map(SymTable* top, void (*apply)(SymTable*, void*), void* arg);

/*
Base Types:
    bool
    int
    uint
    u8 u16 u32 u64
    i8 i16 i32 i64
    uintptr intptr
    rune
    byte
    string

Reference:
    u8&

Array:
    u8[] u8[10] u8[10][10]

Function:
    int (string[])

Struct:

Union:


Examples:
def main(args)
    return 0;
end

def main(args : string[]) : int
    return 0;
end

def main : int(string[])
    fn(args)

    end
end

def foo (123 : int);
def foo : int 123;
def foo 123;

*/


/* Pretty Printing
 *****************************************************************************/
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
void pprint_tree(FILE* file, AST* tree, int depth);

/* Lexer and Parser Types
 *****************************************************************************/
typedef struct {
    char* line;
    size_t index;
    size_t lineno;
    FILE* input;
    char* prompt;
    Tok* tok;
} Parser;

// Lexer routines
Tok* gettoken(Parser* ctx);
void fetchline(Parser* ctx);

// Parser routines
Parser* parser_new(char* p_prompt, FILE* input);

// Grammar Routines
AST* toplevel(Parser* p);

// Compiler Passes
AST* normalize(AST* tree);
void codegen(FILE* file, AST* tree);

#endif /* SCLPL_H */
