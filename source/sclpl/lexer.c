/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "lexer.h"
#include <string.h>

bool lexer_oneof(const char* class, char c) {
    return false;
}

char* lexer_dup(const char* p_old) {
    size_t length = strlen(p_old);
    char* p_str = (char*)malloc(length+1);
    memcpy(p_str, p_old, length);
    p_str[length] = '\0';
    return p_str;
}

lex_tok_t* lex_tok_new(lex_tok_type_t type, void* val) {
    lex_tok_t* p_tok = (lex_tok_t*)malloc(sizeof(lex_tok_t));
    p_tok->type  = type;
    p_tok->value = val;
    return p_tok;
}

lex_tok_t* lexer_make_token(char* text);
lex_tok_t* lexer_punc(char* text);
lex_tok_t* lexer_char(char* text);
lex_tok_t* lexer_radix_int(char* text);
lex_tok_t* lexer_number(char* text);
lex_tok_t* lexer_bool(char* text);
lex_tok_t* lexer_var(char* text);

lexer_t* lexer_new(char* p_prompt, FILE* p_input) {
    lexer_t* p_lexer = (lexer_t*)malloc(sizeof(lexer_t));
    p_lexer->scanner = scanner_new(p_prompt, p_input);
    return p_lexer;
}

lex_tok_t* lexer_read(lexer_t* p_lexer) {
    mpc_result_t r;
    lex_tok_t* p_tok = NULL;
    char* text = scanner_read(p_lexer->scanner);
    if (NULL != text) {
        p_tok = lexer_make_token(text);
        free(text);
    }
    return p_tok;
}

lex_tok_t* lexer_make_token(char* text) {
    lex_tok_t* p_tok = NULL;
    if (lexer_oneof("()[];,'\"", text[0])) {
        p_tok = lexer_punc(text);
    } else if (text[0] == '\\') {
        p_tok = lexer_char(text);
    } else if ((text[0] == '0') && lexer_oneof("bodx",text[1])) {
        p_tok = lexer_radix_int(text);
    } else if (lexer_oneof("+-0123456789",text[0])) {
        p_tok = lexer_number(text);
    } else if ((0 == strcmp(text,"true")) || (0 == strcmp(text,"false"))) {
        p_tok = lexer_bool(text);
    } else {
        p_tok = lexer_var(text);
    }
    return p_tok;
}


lex_tok_t* lexer_punc(char* text)
{
    lex_tok_t* p_tok = NULL;
    switch (text[0]) {
        case '(': p_tok = lex_tok_new(T_LPAR,   NULL); break;
        case ')': p_tok = lex_tok_new(T_RPAR,   NULL); break;
        case '{': p_tok = lex_tok_new(T_LBRACE, NULL); break;
        case '}': p_tok = lex_tok_new(T_RBRACE, NULL); break;
        case '[': p_tok = lex_tok_new(T_LBRACK, NULL); break;
        case ']': p_tok = lex_tok_new(T_RBRACK, NULL); break;
        case ';': p_tok = lex_tok_new(T_END,    NULL); break;
        case ',': p_tok = lex_tok_new(T_COMMA,  NULL); break;
    }
    return p_tok;
}

lex_tok_t* lexer_char(char* text)
{
    lex_tok_t* p_tok = NULL;
    static const char* lookup_table[5] = {
        " \0space",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab"
    };
    if (strlen(text) == 1) {
        p_tok = lex_tok_new(T_CHAR, (void*)(text[0]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (strcmp(text, &(lookup_table[i][2]))) {
                p_tok = lex_tok_new(T_CHAR, (void*)(lookup_table[i][0]));
                break;
            }
        }
    }
    return p_tok;
}

lex_tok_t* lexer_radix_int(char* text)
{
    return NULL;
}

lex_tok_t* lexer_number(char* text)
{
    return NULL;
}

lex_tok_t* lexer_bool(char* text)
{
    return lex_tok_new(T_BOOL, (void*)((0 == strcmp(text,"true")) ? true : false));
}

lex_tok_t* lexer_var(char* text)
{
    return lex_tok_new(T_VAR, lexer_dup(text));
}

#if 0
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
        case '(': p_tok = lex_tok_new(T_LPAR,   NULL); break;
        case ')': p_tok = lex_tok_new(T_RPAR,   NULL); break;
        case '{': p_tok = lex_tok_new(T_LBRACE, NULL); break;
        case '}': p_tok = lex_tok_new(T_RBRACE, NULL); break;
        case '[': p_tok = lex_tok_new(T_LBRACK, NULL); break;
        case ']': p_tok = lex_tok_new(T_RBRACK, NULL); break;
        case ';': p_tok = lex_tok_new(T_END,    NULL); break;
        case ',': p_tok = lex_tok_new(T_COMMA,  NULL); break;
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
    return lex_tok_new(T_INT, p_int);
}

lex_tok_t* lexer_float(mpc_ast_t* p_tok_ast)
{
    double* p_dbl = (double*)malloc(sizeof(double));
    errno = 0;
    *p_dbl = strtod(p_tok_ast->contents, NULL);
    assert(errno == 0);
    return lex_tok_new(T_FLOAT, p_dbl);
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
        p_tok = lex_tok_new(T_CHAR, (void*)(p_tok_ast->contents[0]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (strcmp(p_tok_ast->contents, &(lookup_table[i][2]))) {
                p_tok = lex_tok_new(T_CHAR, (void*)(lookup_table[i][0]));
                break;
            }
        }
    }
    return p_tok;
}

lex_tok_t* lexer_bool(mpc_ast_t* p_tok_ast)
{
    return lex_tok_new(T_BOOL, (void*)((0==strcmp(p_tok_ast->contents,"True")) ? true : false));
}

lex_tok_t* lexer_var(mpc_ast_t* p_tok_ast)
{
    char* p_str = lexer_dup(p_tok_ast->contents);
    return lex_tok_new(T_VAR, p_str);
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
#endif
