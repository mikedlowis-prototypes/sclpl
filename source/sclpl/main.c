#include "mpc.h"
#include "scanner.h"
#include "lexer.h"
#include "opts.h"
#include <stdio.h>

/*****************************************************************************/
typedef struct {
    lexer_t* p_lexer;
    lex_tok_t* p_tok;
} parser_t;

lex_tok_t tok_eof = { T_END_FILE, NULL, 0, 0, NULL };

parser_t* parser_new(char* p_prompt, FILE* input)
{
    parser_t* p_parser = (parser_t*)malloc(sizeof(parser_t));
    p_parser->p_lexer = lexer_new(p_prompt, input);
    p_parser->p_tok = NULL;
    return p_parser;
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
        p_parser->p_tok = NULL;
        ret = true;
    }
    return ret;
}

bool parser_accept_str(parser_t* p_parser, lex_tok_type_t type, const char* p_text)
{
    bool ret = false;
    if ((parser_peek(p_parser)->type == type) && (0 == strcmp((char*)(p_parser->p_tok->value), p_text))) {
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

/*****************************************************************************/
void parser_toplevel(parser_t* p_parser);
void parser_import(parser_t* p_parser);
void parser_definition(parser_t* p_parser);
void parser_expression(parser_t* p_parser);
void parser_literal(parser_t* p_parser);
void parser_arglist(parser_t* p_parser);
void parser_if_stmnt(parser_t* p_parser);
void parser_fn_stmnt(parser_t* p_parser);

void parser_toplevel(parser_t* p_parser)
{
    if (parser_accept_str(p_parser, T_VAR, "import"))
        parser_import(p_parser);
    else if (parser_accept_str(p_parser, T_VAR, "def"))
        parser_definition(p_parser);
    else if (p_parser->p_lexer->scanner->p_input == stdin)
        parser_expression(p_parser);
    else
        parser_error(p_parser, "Unrecognized top-level form");
}

void parser_import(parser_t* p_parser)
{
    parser_expect(p_parser, T_VAR);
    parser_expect(p_parser, T_END);
}

void parser_definition(parser_t* p_parser)
{
    parser_expect(p_parser,T_VAR);
    if (parser_peek(p_parser)->type == T_LPAR) {
        parser_fn_stmnt(p_parser);
    } else {
        parser_expression(p_parser);
        parser_expect(p_parser,T_END);
    }
}

void parser_expression(parser_t* p_parser)
{
    if (parser_accept(p_parser, T_LPAR)) {
        parser_expression(p_parser);
        parser_accept(p_parser, T_RPAR);
    } else if (parser_accept_str(p_parser, T_VAR, "if")) {
        parser_if_stmnt(p_parser);
    } else if (parser_accept_str(p_parser, T_VAR, "fn")) {
        parser_fn_stmnt(p_parser);
    } else if (parser_peek(p_parser)->type == T_VAR) {
        parser_expect(p_parser, T_VAR);
        if (parser_peek(p_parser)->type == T_LPAR) {
            parser_arglist(p_parser);
        }
    } else {
        parser_literal(p_parser);
    }
}

void parser_literal(parser_t* p_parser)
{
    switch (parser_peek(p_parser)->type)
    {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            parser_accept(p_parser, parser_peek(p_parser)->type);
            break;

        default:
            parser_error(p_parser, "Not a valid expression");
            break;
    }
}

void parser_arglist(parser_t* p_parser)
{
    parser_expect(p_parser, T_LPAR);
    while(parser_peek(p_parser)->type != T_RPAR) {
        parser_expression(p_parser);
        if(parser_peek(p_parser)->type != T_RPAR)
            parser_expect(p_parser, T_COMMA);
    }
    parser_expect(p_parser, T_RPAR);
}

void parser_if_stmnt(parser_t* p_parser)
{
    parser_expression(p_parser);
    parser_expression(p_parser);
    parser_expect_str(p_parser,T_VAR,"else");
    parser_expression(p_parser);
    parser_expect(p_parser,T_END);
}

void parser_fn_stmnt(parser_t* p_parser)
{
    parser_expect(p_parser, T_LPAR);
    while(parser_peek(p_parser)->type != T_RPAR) {
        parser_expect(p_parser, T_VAR);
        if(parser_peek(p_parser)->type != T_RPAR)
            parser_expect(p_parser, T_COMMA);
    }
    parser_expect(p_parser, T_RPAR);
    while(parser_peek(p_parser)->type != T_END) {
        parser_expression(p_parser);
    }
    parser_expect(p_parser, T_END);
}

/* SCLPL Parser
 *****************************************************************************/
/* TODO:

    * Formalize grammar for parser
    * Paren for function application must be on same line as variable in REPL
    * "end" and ';' must be equivalent
    * skip line on error and terminate after full program parse
    * skip line and print on error but do not terminate the REPL
    * Phase out use of MPC
    * Integrate libcds
    * Integrate command line parsing

*/

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    parser_t* p_parser = parser_new(":> ", stdin);
    while(!parser_eof(p_parser)) {
        parser_toplevel(p_parser);
        puts("OK.");
    }

    return 0;
}
