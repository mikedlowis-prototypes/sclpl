/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <libparse.h>

DEFINE_EXCEPTION(ParseException, &RuntimeException);

Tok tok_eof = { NULL, 0, 0, T_END_FILE, {0} };

static void parser_free(void* obj) {
    Parser* parser = (Parser*)obj;
    if ((NULL != parser->tok) && (&tok_eof != parser->tok)) {
        mem_release(parser->tok);
    }
    mem_release(parser->tokbuf);
}

Parser* parser_new(char* prompt, FILE* input)
{
    Parser* parser  = (Parser*)mem_allocate(sizeof(Parser), &parser_free);
    parser->line    = NULL;
    parser->index   = 0;
    parser->lineno  = 0;
    parser->input   = input;
    parser->prompt  = prompt;
    parser->tok     = NULL;
    parser->tokbuf = vec_new(0);
    return parser;
}

void fetch(Parser* parser)
{
    parser->tok = gettoken(parser);
    if (NULL == parser->tok)
        parser->tok = &tok_eof;
}

Tok* peek(Parser* parser)
{
    if (NULL == parser->tok)
        fetch(parser);
    return parser->tok;
}

bool parser_eof(Parser* parser) {
    return (peek(parser)->type == T_END_FILE);
}

void parser_resume(Parser* parser) {
    if ((NULL != parser->tok) && (&tok_eof != parser->tok)) {
        mem_release(parser->tok);
        parser->tok = NULL;
    }
    vec_clear(parser->tokbuf);
    skipline(parser);
}

void error(Parser* parser, const char* text)
{
    (void)parser;
    Tok* tok = peek(parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, text);
    throw_msg(ParseException, text);
}

bool accept(Parser* parser, TokType type)
{
    bool ret = false;
    if (peek(parser)->type == type) {
        vec_push_back(parser->tokbuf, tree_new(ATOM, parser->tok));
        parser->tok = NULL;
        ret = true;
    }
    return ret;
}

bool accept_str(Parser* parser, TokType type, const char* text)
{
    bool ret = false;
    if ((peek(parser)->type == type) && (0 == strcmp((char*)(parser->tok->value.text), text))) {
        vec_push_back(parser->tokbuf, tree_new(ATOM, parser->tok));
        parser->tok = NULL;
        ret = true;
    }
    return ret;
}

bool expect(Parser* parser, TokType type)
{
    bool ret = false;
    if (accept(parser, type)) {
        ret = true;
    } else {
        error(parser, "Unexpected token");
    }
    return ret;
}

bool expect_str(Parser* parser, TokType type, const char* text)
{
    bool ret = false;
    if (accept_str(parser, type, text)) {
        ret = true;
    } else {
        error(parser, "Unexpected token");
    }
    return ret;
}

size_t mark(Parser* parser)
{
    return (vec_size(parser->tokbuf) - 1);
}

void reduce(Parser* parser, size_t mark)
{
    vec_t* buf  = parser->tokbuf;
    vec_t* form = vec_new(0);
    for(size_t idx = mark; idx < vec_size(buf); idx++) {
        AST* tree = mem_retain(vec_at(buf, idx));
        vec_push_back(form, tree);
    }
    vec_erase(buf, mark, vec_size(buf)-1);
    vec_push_back(buf, tree_new(TREE, form));
}

AST* get_tree(Parser* parser) {
    AST* tree = NULL;
    if (1 == vec_size(parser->tokbuf)) {
        tree = mem_retain(vec_at(parser->tokbuf, 0));
        vec_clear(parser->tokbuf);
    } else {
        tree = tree_new(TREE, parser->tokbuf);
        parser->tokbuf = vec_new(0);
    }
    return tree;
}

//void insert(Parser* parser, TokType type, char* value) {
//    Tok* tok = token(type, strdup(value));
//    AST*   tree = tree_new(ATOM, tok);
//    vec_push_back(parser->tokbuf, tree);
//}

