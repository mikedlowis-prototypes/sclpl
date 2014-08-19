/**
  @file ast.c
  @brief See header for details
  $Revision$
  $HeadURL$
*/
#include "ast.h"
#include <stdlib.h>

void ast_free(void* p_obj)
{
    (void)p_obj;
}

ast_t* ast_new(ast_type_t type, void* value)
{
    ast_t* p_ast = (ast_t*)malloc(sizeof(ast_t));
    p_ast->pos = NULL;
    p_ast->type = type;
    p_ast->value = value;
    p_ast->children = NULL;
    return p_ast;
}

void ast_set_pos(ast_t* p_ast, const char* file, size_t line, size_t col)
{
    pos_t* p_pos = (pos_t*)malloc(sizeof(pos_t));
    p_pos->file = file;
    p_pos->line = line;
    p_pos->column = col;
    p_ast->pos = p_pos;
}

const pos_t* ast_get_pos(ast_t* p_ast)
{
    return p_ast->pos;
}

void ast_set_type(ast_t* p_ast, ast_type_t type)
{
    p_ast->type = type;
}

ast_type_t ast_get_type(ast_t* p_ast)
{
    return p_ast->type;
}

void ast_set_value(ast_t* p_ast, void* value)
{
    p_ast->value = value;
}

const void* ast_get_value(ast_t* p_ast)
{
    return p_ast->value;
}

ast_t* ast_add_child(ast_t* p_ast, ast_t* p_child)
{
    child_t* child = p_ast->children;
    child_t* newchild = (child_t*)malloc(sizeof(child));
    newchild->ast = p_ast;
    newchild->next = NULL;
    if (child != NULL) {
        while (child->next != NULL) child = child->next;
        child->next = newchild;
    } else {
        p_ast->children = newchild;
    }
}

ast_t* ast_map(const ast_t* p_ast, ast_map_fn_t p_fn)
{
    (void)p_ast;
    (void)p_fn;
    return NULL;
}

