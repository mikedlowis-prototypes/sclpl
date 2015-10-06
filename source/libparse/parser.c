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
    mem_release(parser->stack);
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
    parser->stack = vec_new(0);
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
    vec_clear(parser->stack);
    /* We ignore the rest of the current line and attempt to start parsing
     * again on the next line */
    fetchline(parser);
}

void error(Parser* parser, const char* text)
{
    (void)parser;
    Tok* tok = peek(parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, text);
    throw_msg(ParseException, text);
}

Tok* shifttok(Parser* parser, TokType type)
{
    Tok* tok = NULL;
    if (peek(parser)->type == type) {
        vec_push_back(parser->stack, parser->tok);
        parser->tok = NULL;
    } else {
        error(parser, "Unexpected token");
    }
    return tok;
}

bool accept(Parser* parser, TokType type)
{
    bool ret = false;
    if (peek(parser)->type == type) {
        mem_swap((void**)&(parser->tok), NULL);
        ret = true;
    }
    return ret;
}

bool accept_str(Parser* parser, TokType type, const char* text)
{
    bool ret = false;
    if ((peek(parser)->type == type) && (0 == strcmp((char*)(parser->tok->value.text), text))) {
        mem_swap((void**)&(parser->tok), NULL);
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

size_t stack_push(Parser* ctx, AST* node)
{
    vec_push_back(ctx->stack, node);
    return vec_size(ctx->stack)-1;
}

AST* stack_pop(Parser* ctx)
{
    return (AST*)vec_pop_back(ctx->stack);
}

AST* stack_get(Parser* ctx, int index)
{
    index = (index < 0) ? (vec_size(ctx->stack)+index) : index;
    return (AST*)vec_at(ctx->stack, (size_t)index);
}

