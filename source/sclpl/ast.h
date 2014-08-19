/**
  @file ast.h
  @brief TODO: Describe this file
  $Revision$
  $HeadURL$
  */
#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <string.h>

typedef enum {
    BOOLEAN,
    INTEGER,
    FLOAT,
    CHARACTER,
    STRING,
    SEXPR,
    UNKNOWN
} ast_type_t;

typedef struct {
    const char* file;
    size_t line;
    size_t column;
} pos_t;

struct ast_t;

typedef struct child_t {
    struct ast_t* ast;
    struct child_t* next;
} child_t;

typedef struct ast_t {
    pos_t* pos;
    ast_type_t type;
    void* value;
    child_t* children;
} ast_t;

typedef ast_t* (*ast_map_fn_t)(const ast_t* p_ast);

void ast_free(void* p_obj);

ast_t* ast_new(ast_type_t type, void* value);

void ast_set_pos(ast_t* p_ast, const char* file, size_t line, size_t col);

const pos_t* ast_get_pos(ast_t* p_ast);

void ast_set_type(ast_t* p_ast, ast_type_t type);

ast_type_t ast_get_type(ast_t* p_ast);

void ast_set_value(ast_t* p_ast, void* value);

const void* ast_get_value(ast_t* p_ast);

ast_t* ast_add_child(ast_t* p_ast, ast_t* p_child);

ast_t* ast_map(const ast_t* p_ast, ast_map_fn_t p_fn);


#endif /* AST_H */
