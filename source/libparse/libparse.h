/**
  @file libparse.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef LIBPARSE_H
#define LIBPARSE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "mem.h"
#include "vec.h"
#include "exn.h"

/* Token Types
 *****************************************************************************/
#if 1

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

Tok* Token(const char* file, size_t line, size_t col, char* text);

#endif

/* AST Types
 *****************************************************************************/

/* Lexer and Parser Types
 *****************************************************************************/
//typedef enum {
//    T_ID, T_CHAR, T_INT, T_FLOAT, T_BOOL, T_STRING, T_LBRACE, T_RBRACE, T_LBRACK,
//    T_RBRACK, T_LPAR, T_RPAR, T_COMMA, T_SQUOTE, T_DQUOTE, T_END, T_END_FILE
//} TokenType;
//
//typedef struct {
//    TokenType type;
//    const char* file;
//    size_t line;
//    size_t col;
//    void* value;
//} Token;

DECLARE_EXCEPTION(ParseException);

typedef struct {
    char* line;
    size_t index;
    size_t lineno;
    FILE* input;
    char* prompt;
    Tok* tok;
    vec_t* tokbuf;
} Parser;

typedef enum {
    ATOM,
    TREE
} ASTTag;

typedef struct {
    ASTTag tag;
    union {
        Tok* tok;
        vec_t* vec;
    } ptr;
} AST;

// Lexer routines
Tok* gettoken(Parser* ctx);

//Lexer* lexer_new(char* p_prompt, FILE* p_input);
//Token* token(TokenType type, void* val);
//Token* lexer_read(Lexer* p_lexer);
//void lexer_skipline(Lexer* p_lexer);
//char* lexer_dup(const char* p_old);

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
//size_t mark(Parser* p_parser);
//void reduce(Parser* p_parser, size_t mark);
//AST* get_tree(Parser* p_parser);
//void insert(Parser* p_parser, TokType type, char* value);

// AST Routines
AST* tree_convert(AST* p_tree);
AST* tree_new(ASTTag tag, void* p_obj);
AST* tree_get_child(AST* p_tree, size_t idx);
void* tree_get_val(AST* p_tree);
void* tree_get_child_val(AST* p_tree, size_t idx);
bool tree_is_formtype(AST* p_tree, const char* val);

// Grammar Routines
AST* toplevel(Parser* p);

#endif /* LIBPARSE_H */
