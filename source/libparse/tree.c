/**
  @file tree.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "tree.h"

static void tree_free(void* p_obj) {
    tree_t* p_tree = ((tree_t*)p_obj);
    if (NULL != p_tree->ptr.tok) {
        mem_release(p_tree->ptr.tok);
    }
}

static bool is_punctuation(lex_tok_t* p_tok) {
    bool ret = false;
    switch(p_tok->type) {
        case T_END:
        case T_LBRACE:
        case T_RBRACE:
        case T_LBRACK:
        case T_RBRACK:
        case T_LPAR:
        case T_RPAR:
        case T_COMMA:
            ret = true;
        default:
            break;
    }
    return ret;
}

tree_t* tree_convert(tree_t* p_tree) {
    tree_t* p_newtree = NULL;
    if (p_tree->tag == ATOM) {
        if (!is_punctuation(p_tree->ptr.tok))
            p_newtree = mem_retain(p_tree);
    } else {
        vec_t* p_vec = p_tree->ptr.vec;
        vec_t* p_newvec = vec_new(0);
        p_newtree = tree_new(TREE, p_newvec);
        for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
            tree_t* p_item = tree_convert(vec_at(p_vec,idx));
            if (NULL != p_item)
                vec_push_back(p_newvec, p_item);
        }
    }
    return p_newtree;
}

tree_t* tree_new(tree_tag_t tag, void* p_obj) {
    tree_t* p_tree = (tree_t*)mem_allocate(sizeof(tree_t), &tree_free);
    p_tree->tag     = tag;
    p_tree->ptr.tok = (lex_tok_t*)p_obj;
    return p_tree;
}

tree_t* tree_get_child(tree_t* p_tree, size_t idx) {
    tree_t* child = NULL;
    if (p_tree->tag == TREE) {
        vec_t* vec = p_tree->ptr.vec;
        if (idx < vec_size(vec))
            child = vec_at(vec, idx);
    }
    return child;
}

void* tree_get_val(tree_t* p_tree) {
    void* ret = NULL;
    if (p_tree->tag == ATOM) {
        ret = p_tree->ptr.tok->value;
    }
    return ret;
}

void* tree_get_child_val(tree_t* p_tree, size_t idx) {
    void* ret = NULL;
    tree_t* child = tree_get_child(p_tree,idx);
    if (child != NULL) {
        ret = tree_get_val(child);
    }
    return ret;
}

bool tree_is_formtype(tree_t* p_tree, const char* val) {
    bool ret = false;
    tree_t* child = tree_get_child(p_tree, 0);
    if ((NULL != child) && (child->tag == ATOM)) {
        lex_tok_t* token = child->ptr.tok;
        if ((token->type == T_ID) &&
            (0 == strcmp(val, (char*)token->value))) {
            ret = true;
        }
    }
    return ret;
}


tree_t* tree_walk(tree_t* tree, tree_walker_t* walker)
{
    size_t idx;
    tree_t* new_node;
    new_node = walker->fn(walker->env, tree, PRE_NODE);
    if (tree->tag == TREE) {
        walker->fn(walker->env, tree, PRE_CHILDREN);
        for (idx = 0; idx < vec_size(tree->ptr.vec); idx++) {
            tree_t* child = vec_at(tree->ptr.vec, idx);
            walker->fn(walker->env, tree, PRE_CHILD);
            tree_t* new_child = tree_walk( child, walker );
            walker->fn(walker->env, tree, POST_CHILD);
            if ((NULL != new_node) && (new_node->tag == TREE) && (NULL != new_child)) {
                vec_push_back(new_node->ptr.vec, new_child);
            }
        }
        walker->fn(walker->env, tree, POST_CHILDREN);
    }
    return new_node;
}

tree_walker_t* tree_walker(void* env, tree_walk_fn_t fn)
{
    tree_walker_t* p_walker = (tree_walker_t*)mem_allocate(sizeof(tree_walker_t),NULL);
    p_walker->env = env;
    p_walker->fn  = fn;
    return p_walker;
}

