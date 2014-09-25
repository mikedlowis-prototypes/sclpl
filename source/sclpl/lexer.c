/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "lexer.h"
#include <string.h>
#include <stdlib.h>

bool lexer_oneof(const char* class, char c) {
    bool ret = false;
    size_t sz = strlen(class);
    for (size_t idx = 0; idx < sz; idx++) {
        if (c == class[idx]) {
            ret = true;
            break;
        }
    }
    return ret;
}

bool is_float(char* text) {
    while (text[0] != '\0')
        if (text[0] == '.')
            return true;
        else
            text++;
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

static int read_radix(char ch) {
    switch(ch) {
        case 'b': return 2;
        case 'o': return 8;
        case 'd': return 10;
        case 'h': return 16;
        default:  return 10;
    }
}

lex_tok_t* lexer_make_token(char* text);
lex_tok_t* lexer_punc(char* text);
lex_tok_t* lexer_char(char* text);
lex_tok_t* lexer_radix_int(char* text);
lex_tok_t* lexer_number(char* text);
lex_tok_t* lexer_integer(char* text, int base);
lex_tok_t* lexer_float(char* text);
lex_tok_t* lexer_bool(char* text);
lex_tok_t* lexer_var(char* text);

char* lexer_tok_type_str(lex_tok_t* p_tok) {
    switch(p_tok->type) {
        case T_END:      return "T_END";
        case T_STRING:   return "T_STRING";
        case T_CHAR:     return "T_CHAR";
        case T_INT:      return "T_INT";
        case T_FLOAT:    return "T_FLOAT";
        case T_BOOL:     return "T_BOOL";
        case T_LBRACE:   return "T_LBRACE";
        case T_RBRACE:   return "T_RBRACE";
        case T_LBRACK:   return "T_LBRACK";
        case T_RBRACK:   return "T_RBRACK";
        case T_LPAR:     return "T_LPAR";
        case T_RPAR:     return "T_RPAR";
        case T_COMMA:    return "T_COMMA";
        case T_VAR:      return "T_VAR";
        case T_END_FILE: return "T_END_FILE";
        default:         return NULL;
    }
}

lexer_t* lexer_new(char* p_prompt, FILE* p_input) {
    lexer_t* p_lexer = (lexer_t*)malloc(sizeof(lexer_t));
    p_lexer->scanner = scanner_new(p_prompt, p_input);
    return p_lexer;
}

lex_tok_t* lexer_read(lexer_t* p_lexer) {
    lex_tok_t* p_tok = NULL;
    char* text = scanner_read(p_lexer->scanner);
    if (NULL != text) {
        p_tok = lexer_make_token(text);
        if (NULL != p_tok)
            printf("TOK: '%s' -> %s\n", text, lexer_tok_type_str(p_tok));
        free(text);
    }
    return p_tok;
}

lex_tok_t* lexer_make_token(char* text) {
    lex_tok_t* p_tok = NULL;
    if ((0 == strcmp(text,"end") || (text[0] == ';'))) {
        p_tok = lex_tok_new(T_END, NULL);
    } else if (lexer_oneof("()[];,'\"", text[0])) {
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
        p_tok = lex_tok_new(T_CHAR, (void*)((intptr_t)text[0]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (strcmp(text, &(lookup_table[i][2]))) {
                p_tok = lex_tok_new(T_CHAR, (void*)((intptr_t)lookup_table[i][0]));
                break;
            }
        }
    }
    return p_tok;
}

lex_tok_t* lexer_radix_int(char* text)
{
    return lexer_integer(text, read_radix(text[1]));
}

lex_tok_t* lexer_number(char* text)
{
    if (is_float(text))
        return lexer_integer(text, 10);
    else
        return lexer_float(text);
}

lex_tok_t* lexer_integer(char* text, int base)
{
    long* p_int = (long*)malloc(sizeof(long));
    errno = 0;
    *p_int = strtol(text, NULL, base);
    assert(errno == 0);
    return lex_tok_new(T_INT, p_int);
}

lex_tok_t* lexer_float(char* text)
{
    double* p_dbl = (double*)malloc(sizeof(double));
    errno = 0;
    *p_dbl = strtod(text, NULL);
    assert(errno == 0);
    return lex_tok_new(T_FLOAT, p_dbl);
}

lex_tok_t* lexer_bool(char* text)
{
    return lex_tok_new(T_BOOL, (void*)((0 == strcmp(text,"true")) ? true : false));
}

lex_tok_t* lexer_var(char* text)
{
    return lex_tok_new(T_VAR, lexer_dup(text));
}

