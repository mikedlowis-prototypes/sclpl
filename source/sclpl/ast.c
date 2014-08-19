/**
  @file ast.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include "ast.h"

void ast_free(void* p_obj)
{
    (void)p_obj;
}

ast_t* ast_new(ast_type_t type, void* value)
{
    (void)type;
    (void)value;
    return NULL;
}

void ast_set_pos(ast_t* p_ast, const char* file, size_t line, size_t col)
{
    (void)p_ast;
    (void)file;
    (void)line;
    (void)col;
}

const pos_t* ast_get_pos(ast_t* p_ast)
{
    (void)p_ast;
    return NULL;
}

void ast_set_type(ast_t* p_ast, ast_type_t type)
{
    (void)p_ast;
    (void)type;
}

ast_type_t ast_get_type(ast_t* p_ast)
{
    (void)p_ast;
    return UNKNOWN;
}

void ast_set_value(ast_t* p_ast, void* value)
{
    (void)p_ast;
    (void)value;
}

const void* ast_get_value(ast_t* p_ast)
{
    (void)p_ast;
    return NULL;
}

void ast_set_children(ast_t* p_ast, child_t* p_children)
{
    (void)p_ast;
    (void)p_children;
}

const child_t* ast_get_children(ast_t* p_ast)
{
    (void)p_ast;
    return NULL;
}

ast_t* ast_map(const ast_t* p_ast, ast_map_fn_t p_fn)
{
    (void)p_ast;
    (void)p_fn;
    return NULL;
}
