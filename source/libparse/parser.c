/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <libparse.h>

DEFINE_EXCEPTION(ParseException, &RuntimeException);

Token tok_eof = { T_END_FILE, NULL, 0, 0, NULL };

static void parser_free(void* p_obj) {
    Parser* p_parser = (Parser*)p_obj;
    if ((NULL != p_parser->p_tok) && (&tok_eof != p_parser->p_tok)) {
        mem_release(p_parser->p_tok);
    }
    mem_release(p_parser->p_lexer);
    mem_release(p_parser->p_tok_buf);
}

Parser* parser_new(char* p_prompt, FILE* input)
{
    Parser* p_parser = (Parser*)mem_allocate(sizeof(Parser), &parser_free);
    p_parser->p_lexer = lexer_new(p_prompt, input);
    p_parser->p_tok = NULL;
    p_parser->p_tok_buf = vec_new(0);
    return p_parser;
}

void fetch(Parser* p_parser)
{
    p_parser->p_tok = lexer_read(p_parser->p_lexer);
    if (NULL == p_parser->p_tok)
        p_parser->p_tok = &tok_eof;
}

Token* peek(Parser* p_parser)
{
    if (NULL == p_parser->p_tok)
        fetch(p_parser);
    return p_parser->p_tok;
}

bool parser_eof(Parser* p_parser) {
    return (peek(p_parser)->type == T_END_FILE);
}

void parser_resume(Parser* p_parser) {
    if ((NULL != p_parser->p_tok) && (&tok_eof != p_parser->p_tok)) {
        mem_release(p_parser->p_tok);
        p_parser->p_tok = NULL;
    }
    vec_clear(p_parser->p_tok_buf);
    lexer_skipline(p_parser->p_lexer);
}

void error(Parser* p_parser, const char* p_text)
{
    (void)p_parser;
    Token* tok = peek(p_parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, p_text);
    throw_msg(ParseException, p_text);
}

bool accept(Parser* p_parser, TokenType type)
{
    bool ret = false;
    if (peek(p_parser)->type == type) {
        vec_push_back(p_parser->p_tok_buf, tree_new(ATOM, p_parser->p_tok));
        p_parser->p_tok = NULL;
        ret = true;
    }
    return ret;
}

bool accept_str(Parser* p_parser, TokenType type, const char* p_text)
{
    bool ret = false;
    if ((peek(p_parser)->type == type) && (0 == strcmp((char*)(p_parser->p_tok->value), p_text))) {
        vec_push_back(p_parser->p_tok_buf, tree_new(ATOM, p_parser->p_tok));
        p_parser->p_tok = NULL;
        ret = true;
    }
    return ret;
}

bool expect(Parser* p_parser, TokenType type)
{
    bool ret = false;
    if (accept(p_parser, type)) {
        ret = true;
    } else {
        error(p_parser, "Unexpected token");
    }
    return ret;
}

bool expect_str(Parser* p_parser, TokenType type, const char* p_text)
{
    bool ret = false;
    if (accept_str(p_parser, type, p_text)) {
        ret = true;
    } else {
        error(p_parser, "Unexpected token");
    }
    return ret;
}

size_t mark(Parser* p_parser)
{
    return (vec_size(p_parser->p_tok_buf) - 1);
}

void reduce(Parser* p_parser, size_t mark)
{
    vec_t* p_buf  = p_parser->p_tok_buf;
    vec_t* p_form = vec_new(0);
    for(size_t idx = mark; idx < vec_size(p_buf); idx++) {
        AST* p_tree = mem_retain(vec_at(p_buf, idx));
        vec_push_back(p_form, p_tree);
    }
    vec_erase(p_buf, mark, vec_size(p_buf)-1);
    vec_push_back(p_buf, tree_new(TREE, p_form));
}

AST* get_tree(Parser* p_parser) {
    AST* p_tree = NULL;
    if (1 == vec_size(p_parser->p_tok_buf)) {
        p_tree = mem_retain(vec_at(p_parser->p_tok_buf, 0));
        vec_clear(p_parser->p_tok_buf);
    } else {
        p_tree = tree_new(TREE, p_parser->p_tok_buf);
        p_parser->p_tok_buf = vec_new(0);
    }
    return p_tree;
}

void insert(Parser* p_parser, TokenType type, void* value) {
    Token* p_tok = token(type, value);
    AST*   p_tree = tree_new(ATOM, p_tok);
    vec_push_back(p_parser->p_tok_buf, p_tree);
}

