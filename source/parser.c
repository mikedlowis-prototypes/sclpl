/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <sclpl.h>

Tok tok_eof = { NULL, 0, 0, T_END_FILE, {0} };

static void parser_free(void* obj) {
    Parser* parser = (Parser*)obj;
    if ((NULL != parser->tok) && (&tok_eof != parser->tok)) {
        gc_delref(parser->tok);
    }
    //gc_delref(parser->stack);
}

Parser* parser_new(char* prompt, FILE* input)
{
    Parser* parser  = (Parser*)gc_alloc(sizeof(Parser), &parser_free);
    parser->line    = NULL;
    parser->index   = 0;
    parser->lineno  = 0;
    parser->input   = input;
    parser->prompt  = prompt;
    parser->tok     = NULL;
    //parser->stack = vec_new(0);
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
        gc_delref(parser->tok);
        parser->tok = NULL;
    }
    //vec_clear(parser->stack);
    /* We ignore the rest of the current line and attempt to start parsing
     * again on the next line */
    fetchline(parser);
}

void error(Parser* parser, const char* text)
{
    (void)parser;
    Tok* tok = peek(parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, text);
    exit(1);
}

bool accept(Parser* parser, TokType type)
{
    bool ret = false;
    if (peek(parser)->type == type) {
        gc_swapref((void**)&(parser->tok), NULL);
        ret = true;
    }
    return ret;
}

bool accept_str(Parser* parser, TokType type, const char* text)
{
    bool ret = false;
    if ((peek(parser)->type == type) && (0 == strcmp((char*)(parser->tok->value.text), text))) {
        gc_swapref((void**)&(parser->tok), NULL);
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

