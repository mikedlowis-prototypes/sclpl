/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "lexer.h"

static lex_tok_t* lexer_translate(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_punc(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_radix(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_integer(mpc_ast_t* p_tok_ast, int base);
static lex_tok_t* lexer_float(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_char(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_bool(mpc_ast_t* p_tok_ast);
static lex_tok_t* lexer_var(mpc_ast_t* p_tok_ast);
static lex_tok_t* lex_tok_new(lex_tok_type_t type, void* val);
static int read_radix(const mpc_ast_t* t);

/* Grammar is auto generated into 'source/grammar.c' */
extern const char Grammar[];

lexer_t* lexer_new(char* p_prompt, FILE* p_input) {
    lexer_t* p_lexer = (lexer_t*)malloc(sizeof(lexer_t));
    /* Build the token parser */
    mpc_parser_t* token     = mpc_new("token");
    mpc_parser_t* atom      = mpc_new("atom");
    mpc_parser_t* punc      = mpc_new("punc");
    mpc_parser_t* floating  = mpc_new("floating");
    mpc_parser_t* integer   = mpc_new("integer");
    mpc_parser_t* radix_num = mpc_new("radixnum");
    mpc_parser_t* character = mpc_new("character");
    mpc_parser_t* boolean   = mpc_new("boolean");
    mpc_parser_t* variable  = mpc_new("var");
    mpca_lang(MPCA_LANG_DEFAULT, Grammar,
        token, atom, punc, floating, integer, radix_num, character, boolean, variable, NULL);
    /* Build the Lexer */
    p_lexer->lexrule = token;
    p_lexer->scanner = scanner_new(p_prompt, p_input);
    return p_lexer;
}

lex_tok_t* lexer_read(lexer_t* p_lexer) {
    mpc_result_t r;
    lex_tok_t* p_tok = NULL;
    char* text = scanner_read(p_lexer->scanner);
    if (NULL != text) {
        if (mpc_parse("<stdin>", text, p_lexer->lexrule, &r)) {
            mpc_ast_print(((mpc_ast_t*)r.output)->children[1]);
            p_tok = lexer_translate( ((mpc_ast_t*)r.output)->children[1] );
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(text);
    }
    return p_tok;
}

lex_tok_t* lexer_translate(mpc_ast_t* p_tok_ast) {
    lex_tok_t* p_tok = (lex_tok_t*)malloc(sizeof(lex_tok_t));
    if (0 == strncmp("atom|punc", p_tok_ast->tag, 9)) {
        p_tok = lexer_punc(p_tok_ast);
    } else if (0 == strncmp("radixnum", p_tok_ast->tag, 8)) {
        p_tok = lexer_radix(p_tok_ast);
    } else if (0 == strncmp("atom|integer", p_tok_ast->tag, 12)) {
        p_tok = lexer_integer(p_tok_ast, 10);
    } else if (0 == strncmp("atom|floating", p_tok_ast->tag, 13)) {
        p_tok = lexer_float(p_tok_ast);
    } else if (0 == strncmp("character", p_tok_ast->tag, 9)) {
        p_tok = lexer_char(p_tok_ast);
    } else if (0 == strncmp("atom|boolean", p_tok_ast->tag, 12)) {
        p_tok = lexer_bool(p_tok_ast);
    } else if (0 == strncmp("atom|var", p_tok_ast->tag, 8)) {
        p_tok = lexer_var(p_tok_ast);
    } else {
        puts("unknown");
    }
    return p_tok;
}

lex_tok_t* lexer_punc(mpc_ast_t* p_tok_ast)
{
    lex_tok_t* p_tok = NULL;
    switch (p_tok_ast->contents[0]) {
        case '(': p_tok = lex_tok_new(LPAR,   NULL); break;
        case ')': p_tok = lex_tok_new(RPAR,   NULL); break;
        case '{': p_tok = lex_tok_new(LBRACE, NULL); break;
        case '}': p_tok = lex_tok_new(RBRACE, NULL); break;
        case '[': p_tok = lex_tok_new(LBRACK, NULL); break;
        case ']': p_tok = lex_tok_new(RBRACK, NULL); break;
        case ';': p_tok = lex_tok_new(END,    NULL); break;
        case ',': p_tok = lex_tok_new(COMMA,  NULL); break;
    }
    return p_tok;
}

lex_tok_t* lexer_radix(mpc_ast_t* p_tok_ast)
{
    return lexer_integer(p_tok_ast->children[1], read_radix(p_tok_ast));
}

lex_tok_t* lexer_integer(mpc_ast_t* p_tok_ast, int base)
{
    long* p_int = (long*)malloc(sizeof(long));
    errno = 0;
    *p_int = strtol(p_tok_ast->contents, NULL, base);
    assert(errno == 0);
    return lex_tok_new(INT, p_int);
}

lex_tok_t* lexer_float(mpc_ast_t* p_tok_ast)
{
    double* p_dbl = (double*)malloc(sizeof(double));
    errno = 0;
    *p_dbl = strtod(p_tok_ast->contents, NULL);
    assert(errno == 0);
    return lex_tok_new(FLOAT, p_dbl);
}

lex_tok_t* lexer_char(mpc_ast_t* p_tok_ast)
{
    lex_tok_t* p_tok = NULL;
    static const char* lookup_table[5] = {
        " \0space",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab"
    };
    if (strlen(p_tok_ast->contents) == 1) {
        p_tok = lex_tok_new(CHAR, (void*)(p_tok_ast->contents[0]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (strcmp(p_tok_ast->contents, &(lookup_table[i][2]))) {
                p_tok = lex_tok_new(CHAR, (void*)(lookup_table[i][0]));
                break;
            }
        }
    }
    return p_tok;
}

lex_tok_t* lexer_bool(mpc_ast_t* p_tok_ast)
{
    return lex_tok_new(BOOL, (void*)((0==strcmp(p_tok_ast->contents,"True")) ? true : false));
}

lex_tok_t* lexer_var(mpc_ast_t* p_tok_ast)
{
    char* p_str = strdup(p_tok_ast->contents);
    return lex_tok_new(VAR, p_str);
}

lex_tok_t* lex_tok_new(lex_tok_type_t type, void* val) {
    lex_tok_t* p_tok = (lex_tok_t*)malloc(sizeof(lex_tok_t));
    p_tok->type  = type;
    p_tok->value = val;
    return p_tok;
}

static int read_radix(const mpc_ast_t* t) {
    switch( t->children[0]->contents[1] ) {
        case 'b': return 2;
        case 'o': return 8;
        case 'd': return 10;
        case 'h': return 16;
        default:  return 10;
    }
}
