/**
  @file tree.h
  @brief Module containing definition and common operations on parse trees.
  $Revision$
  $HeadURL$
  */
#ifndef TREE_H
#define TREE_H

#include "vec.h"
#include "lexer.h"

typedef enum {
    ATOM,
    TREE
} ASTTag;

typedef struct {
    ASTTag tag;
    union {
        Token* tok;
        vec_t* vec;
    } ptr;
} AST;

typedef enum {
    PRE_NODE,
    POST_NODE,
    PRE_CHILDREN,
    POST_CHILDREN,
    PRE_CHILD,
    POST_CHILD,
} tree_walk_pos_t;

typedef AST* (*tree_walk_fn_t)(void* env, AST* node, tree_walk_pos_t pos);

typedef struct {
    void* env;
    tree_walk_fn_t fn;
} tree_walker_t;

AST* tree_convert(AST* p_tree);
AST* tree_new(ASTTag tag, void* p_obj);
AST* tree_get_child(AST* p_tree, size_t idx);
void* tree_get_val(AST* p_tree);
void* tree_get_child_val(AST* p_tree, size_t idx);
bool tree_is_formtype(AST* p_tree, const char* val);
void tree_walk(AST* tree, tree_walker_t* walker);
tree_walker_t* tree_walker(void* env, tree_walk_fn_t fn);

#endif /* TREE_H */
