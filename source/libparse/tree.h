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

typedef enum { ATOM, TREE } tree_tag_t;

typedef struct {
    tree_tag_t tag;
    union {
        lex_tok_t* tok;
        vec_t* vec;
    } ptr;
} tree_t;

typedef enum {
    PRE_NODE,
    POST_NODE,
    PRE_CHILDREN,
    POST_CHILDREN,
    PRE_CHILD,
    POST_CHILD,
} tree_walk_pos_t;

typedef tree_t* (*tree_walk_fn_t)(void* env, tree_t* node, tree_walk_pos_t pos);

typedef struct {
    void* env;
    tree_walk_fn_t fn;
} tree_walker_t;

tree_t* tree_convert(tree_t* p_tree);
tree_t* tree_new(tree_tag_t tag, void* p_obj);
tree_t* tree_get_child(tree_t* p_tree, size_t idx);
void* tree_get_val(tree_t* p_tree);
void* tree_get_child_val(tree_t* p_tree, size_t idx);
bool tree_is_formtype(tree_t* p_tree, const char* val);
tree_t* tree_walk(tree_t* tree, tree_walker_t* walker);
tree_walker_t* tree_walker(void* env, tree_walk_fn_t fn);

#endif /* TREE_H */
