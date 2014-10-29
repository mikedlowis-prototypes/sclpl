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

tree_t* tree_new(tree_tag_t tag, void* p_obj);
tree_t* tree_get_child(tree_t* p_tree, size_t idx);
void* tree_get_val(tree_t* p_tree);
void* tree_get_child_val(tree_t* p_tree, size_t idx);
bool tree_is_formtype(tree_t* p_tree, const char* val);

#endif /* TREE_H */
