/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "parser.h"
#include "vec.h"

lex_tok_t tok_eof = { T_END_FILE, NULL, 0, 0, NULL };

static void parser_free(void* p_obj) {
    parser_t* p_parser = (parser_t*)p_obj;
    if ((NULL != p_parser->p_tok) && (&tok_eof != p_parser->p_tok)) {
        mem_release(p_parser->p_tok);
    }
    mem_release(p_parser->p_lexer);
    mem_release(p_parser->p_tok_buf);
}

parser_t* parser_new(char* p_prompt, FILE* input)
{
    parser_t* p_parser = (parser_t*)mem_allocate(sizeof(parser_t), &parser_free);
    p_parser->p_lexer = lexer_new(p_prompt, input);
    p_parser->p_tok = NULL;
    p_parser->p_tok_buf = vec_new(0);
    return p_parser;
}

static void parser_tree_free(void* p_obj) {
    tree_t* p_tree = ((tree_t*)p_obj);
    if (NULL != p_tree->ptr.tok) {
        mem_release(p_tree->ptr.tok);
    }
}

static tree_t* parser_tree_new(tree_tag_t tag, void* p_obj) {
    tree_t* p_tree = (tree_t*)mem_allocate(sizeof(tree_t), &parser_tree_free);
    p_tree->tag     = tag;
    p_tree->ptr.tok = (lex_tok_t*)p_obj;
    return p_tree;
}

void parser_fetch(parser_t* p_parser)
{
    p_parser->p_tok = lexer_read(p_parser->p_lexer);
    if (NULL == p_parser->p_tok)
        p_parser->p_tok = &tok_eof;
}

lex_tok_t* parser_peek(parser_t* p_parser)
{
    if (NULL == p_parser->p_tok)
        parser_fetch(p_parser);
    return p_parser->p_tok;
}

bool parser_eof(parser_t* p_parser) {
    return (parser_peek(p_parser)->type == T_END_FILE);
}

void parser_error(parser_t* p_parser, const char* p_text)
{
    (void)p_parser;
    fprintf(stderr,"Error: %s\n",p_text);
    exit(1);
}

bool parser_accept(parser_t* p_parser, lex_tok_type_t type)
{
    bool ret = false;
    if (parser_peek(p_parser)->type == type) {
        vec_push_back(p_parser->p_tok_buf, parser_tree_new(ATOM, p_parser->p_tok));
        p_parser->p_tok = NULL;
        ret = true;
    }
    return ret;
}

bool parser_accept_str(parser_t* p_parser, lex_tok_type_t type, const char* p_text)
{
    bool ret = false;
    if ((parser_peek(p_parser)->type == type) && (0 == strcmp((char*)(p_parser->p_tok->value), p_text))) {
        vec_push_back(p_parser->p_tok_buf, parser_tree_new(ATOM, p_parser->p_tok));
        p_parser->p_tok = NULL;
        ret = true;
    }
    return ret;
}

bool parser_expect(parser_t* p_parser, lex_tok_type_t type)
{
    bool ret = false;
    if (parser_accept(p_parser, type)) {
        ret = true;
    } else {
        parser_error(p_parser, "Unexpected token");
    }
    return ret;
}

bool parser_expect_str(parser_t* p_parser, lex_tok_type_t type, const char* p_text)
{
    bool ret = false;
    if (parser_accept_str(p_parser, type, p_text)) {
        ret = true;
    } else {
        parser_error(p_parser, "Unexpected token");
    }
    return ret;
}

size_t parser_mark(parser_t* p_parser)
{
    return (vec_size(p_parser->p_tok_buf) - 1);
}

void parser_reduce(parser_t* p_parser, size_t mark)
{
    vec_t* p_buf  = p_parser->p_tok_buf;
    vec_t* p_form = vec_new(0);
    for(size_t idx = mark; idx < vec_size(p_buf); idx++) {
        tree_tag_t tag = ((tree_t*)vec_at(p_buf, idx))->tag;
        tree_t* p_tree = parser_tree_new(tag, mem_retain(vec_at(p_buf, idx)));
        vec_push_back(p_form, p_tree);
    }
    vec_erase(p_parser->p_tok_buf, mark, vec_size(p_parser->p_tok_buf)-1);
    vec_push_back(p_parser->p_tok_buf, parser_tree_new(TREE, p_form));
}
