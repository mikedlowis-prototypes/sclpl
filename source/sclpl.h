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

// Redfine main
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

/* Token Types
 *****************************************************************************/
typedef enum {
    T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING,
    T_LBRACE, T_RBRACE, T_LBRACK,
    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END,
    T_END_FILE
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
    AST_STRING = 0, AST_SYMBOL, AST_CHAR, AST_INT, AST_FLOAT, AST_BOOL, AST_IDENT,
    AST_REQ, AST_DEF, AST_ANN, AST_IF, AST_FUNC, AST_BLOCK
} ASTType;

typedef struct AST {
    ASTType type;
    union {
        /* Require Node */
        struct {
            char* name;
        } req;
        /* Definition Node */
        struct {
            char* name;
            struct AST* value;
        } def;
        /* Annotation Node */
        struct {
            char* name;
            struct AST* value;
        } ann;
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
        /* Code Block */
        vec_t exprs;
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

/* Code Block */
AST* Block(void);
void block_append(AST* block, AST* expr);
size_t block_size(AST* block);
AST* block_get(AST* block, size_t index);

/* Function */
AST* Func(void);
vec_t* func_args(AST* func);
AST* func_body(AST* func);
void func_add_arg(AST* func, AST* arg);
void func_set_body(AST* func, AST* body);



///* Annotation */
//AST* Ann(char* name, AST* value);
//char* ann_name(AST* def);
//AST* ann_value(AST* def);
//
//
//
//
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
void fetch(Parser* p_parser);
Tok* peek(Parser* p_parser);
bool parser_eof(Parser* p_parser);
void parser_resume(Parser* p_parser);
void error(Parser* p_parser, const char* p_text);
bool match(Parser* parser, TokType type);
bool match_str(Parser* parser, TokType type, const char* text);
Tok* accept(Parser* p_parser, TokType type);
Tok* accept_str(Parser* p_parser, TokType type, const char* p_text);
Tok* expect(Parser* p_parser, TokType type);
Tok* expect_str(Parser* p_parser, TokType type, const char* p_text);

// Grammar Routines
AST* toplevel(Parser* p);

/* Pretty Printing
 *****************************************************************************/
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
void pprint_tree(FILE* file, AST* tree, int depth);

#endif /* SCLPL_H */
