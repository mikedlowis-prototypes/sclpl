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
#include <opts.h>

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

/* Token Types
 *****************************************************************************/
typedef enum {
    T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE, T_RBRACE, T_LBRACK,
    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END, T_END_FILE
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
    AST_REQ, AST_DEF, AST_ANN, AST_IF, AST_FUNC, AST_STRING, AST_SYMBOL,
    AST_CHAR, AST_INT, AST_FLOAT, AST_BOOL, AST_IDENT
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
            struct AST* args;
            struct AST* body;
        } func;
        /* Code Block */
        //vec_t block;
        /* String */
        char* stringval;
        /* Symbol */
        char* symbolval;
        /* Character */
        uint32_t charval;
        /* Integer */
        intptr_t intval;
        /* Float */
        double floatval;
        /* Bool */
        bool boolval;
        /* Ident */
        char* idval;
    } data;
} AST;

/* Require */
AST* Require(char* name);
char* require_name(AST* req);

/* Definition */
AST* Def(char* name, AST* value);
char* def_name(AST* def);
AST* def_value(AST* def);

/* Annotation */
AST* Ann(char* name, AST* value);
char* ann_name(AST* def);
AST* ann_value(AST* def);

/* If Expression */
AST* IfExpr(AST* cond, AST* bthen, AST* belse);
AST* ifexpr_condition(AST* ifexpr);
AST* ifexpr_branch_then(AST* ifexpr);
AST* ifexpr_branch_else(AST* ifexpr);

/* Function */
AST* Func(AST* args, AST* body);
AST* func_args(AST* func);
AST* func_body(AST* func);

/* Code Block */
AST* Block(void);
void block_append(AST* expr);
size_t block_size(AST* block);
AST* block_get(size_t index);

/* String */
AST* String(char* val);
char* string_value(AST* val);

/* Symbol */
AST* Symbol(char* val);
char* symbol_value(AST* val);

/* Character */
AST* Char(uint32_t val);
uint32_t char_value(AST* val);

/* Integer */
AST* Integer(intptr_t val);
intptr_t integer_value(AST* val);

/* Float */
AST* Float(double val);
double float_value(AST* val);

/* Bool */
AST* Bool(bool val);
bool bool_value(AST* val);

/* Ident */
AST* Ident(char* val);
char ident_value(AST* val);

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
bool accept(Parser* p_parser, TokType type);
bool accept_str(Parser* p_parser, TokType type, const char* p_text);
bool expect(Parser* p_parser, TokType type);
bool expect_str(Parser* p_parser, TokType type, const char* p_text);

// Grammar Routines
AST* toplevel(Parser* p);

/* Pretty Printing
 *****************************************************************************/
void pprint_token_type(FILE* file, Tok* token);
void pprint_token_value(FILE* file, Tok* token);
void pprint_token(FILE* file, Tok* token, bool print_loc);
//void pprint_tree(FILE* file, AST* tree, int depth);

#endif /* SCLPL_H */
