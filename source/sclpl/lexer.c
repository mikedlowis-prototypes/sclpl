/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "lexer.h"
#include "mem.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static lex_tok_t* lexer_make_token(size_t line, size_t col, char* text);
static lex_tok_t* lexer_punc(char* text);
static lex_tok_t* lexer_char(char* text);
static lex_tok_t* lexer_radix_int(char* text);
static lex_tok_t* lexer_number(char* text);
static lex_tok_t* lexer_integer(char* text, int base);
static lex_tok_t* lexer_float(char* text);
static lex_tok_t* lexer_bool(char* text);
static lex_tok_t* lexer_var(char* text);
static bool lexer_oneof(const char* class, char c);
static bool is_float(char* text);
static int read_radix(char ch);

static void lex_tok_free(void* p_obj) {
    lex_tok_t* p_tok = (lex_tok_t*)p_obj;
    if ((p_tok->type != T_BOOL) && (p_tok->type != T_CHAR) && (NULL != p_tok->value))
        mem_release(p_tok->value);
}

lex_tok_t* lex_tok_new(lex_tok_type_t type, void* val) {
    lex_tok_t* p_tok = (lex_tok_t*)mem_allocate(sizeof(lex_tok_t), &lex_tok_free);
    p_tok->type  = type;
    p_tok->value = val;
    return p_tok;
}

static void lexer_free(void* p_obj) {
    mem_release(((lexer_t*)p_obj)->scanner);
}

lexer_t* lexer_new(char* p_prompt, FILE* p_input) {
    lexer_t* p_lexer = (lexer_t*)mem_allocate(sizeof(lexer_t), &lexer_free);
    p_lexer->scanner = scanner_new(p_prompt, p_input);
    return p_lexer;
}

lex_tok_t* lexer_read(lexer_t* p_lexer) {
    lex_tok_t* p_tok = NULL;
    size_t line;
    size_t col;
    char* text = scanner_read(p_lexer->scanner, &line, &col);
    if (NULL != text) {
        p_tok = lexer_make_token(line, col, text);
        free(text);
    }
    return p_tok;
}

void lexer_skipline(lexer_t* p_lexer) {
    scanner_getline(p_lexer->scanner);
}

static lex_tok_t* lexer_make_token(size_t line, size_t col, char* text) {
    lex_tok_t* p_tok = NULL;
    if (0 == strcmp(text,"end")) {
        p_tok = lex_tok_new(T_END, NULL);
    } else if (lexer_oneof("()[]{};,'", text[0])) {
        p_tok = lexer_punc(text);
    } else if ('"' == text[0]) {
        text[strlen(text)-1] = '\0';
        p_tok = lex_tok_new(T_STRING, lexer_dup(&text[1]));
    } else if (text[0] == '\\') {
        p_tok = lexer_char(text);
    } else if ((text[0] == '0') && lexer_oneof("bodh",text[1])) {
        p_tok = lexer_radix_int(text);
    } else if (lexer_oneof("+-0123456789",text[0])) {
        p_tok = lexer_number(text);
    } else if ((0 == strcmp(text,"true")) || (0 == strcmp(text,"false"))) {
        p_tok = lexer_bool(text);
    } else {
        p_tok = lexer_var(text);
    }
    /* If we found a valid token then fill in the location details */
    if (NULL != p_tok) {
        p_tok->line = line;
        p_tok->col  = col;
    }
    return p_tok;
}

static lex_tok_t* lexer_punc(char* text)
{
    lex_tok_t* p_tok = NULL;
    switch (text[0]) {
        case '(':  p_tok = lex_tok_new(T_LPAR,   NULL); break;
        case ')':  p_tok = lex_tok_new(T_RPAR,   NULL); break;
        case '{':  p_tok = lex_tok_new(T_LBRACE, NULL); break;
        case '}':  p_tok = lex_tok_new(T_RBRACE, NULL); break;
        case '[':  p_tok = lex_tok_new(T_LBRACK, NULL); break;
        case ']':  p_tok = lex_tok_new(T_RBRACK, NULL); break;
        case ';':  p_tok = lex_tok_new(T_END,    NULL); break;
        case ',':  p_tok = lex_tok_new(T_COMMA,  NULL); break;
        case '\'': p_tok = lex_tok_new(T_SQUOTE, NULL); break;
    }
    return p_tok;
}

static lex_tok_t* lexer_char(char* text)
{
    lex_tok_t* p_tok = NULL;
    static const char* lookup_table[5] = {
        " \0space",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab"
    };
    if (strlen(text) == 2) {
        p_tok = lex_tok_new(T_CHAR, (void*)((intptr_t)text[1]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (0 == strcmp(&text[1], &(lookup_table[i][2]))) {
                p_tok = lex_tok_new(T_CHAR, (void*)((intptr_t)lookup_table[i][0]));
                break;
            }
        }
        if (NULL == p_tok)
            p_tok = lexer_var(text);
    }
    return p_tok;
}

static lex_tok_t* lexer_radix_int(char* text)
{
    lex_tok_t* ret = lexer_integer(&text[2], read_radix(text[1]));
    if (NULL == ret)
        ret = lexer_var(text);
    return ret;
}

static lex_tok_t* lexer_number(char* text)
{
    lex_tok_t* p_tok = NULL;
    if (is_float(text))
        p_tok = lexer_float(text);
    else
        p_tok = lexer_integer(text, 10);
    return (NULL == p_tok) ? lexer_var(text) : p_tok;
}

static lex_tok_t* lexer_integer(char* text, int base)
{
    char* end;
    long* p_int = (long*)mem_allocate(sizeof(long), NULL);
    errno = 0;
    *p_int = strtol(text, &end, base);
    assert(errno == 0);
    return (end[0] == '\0') ? lex_tok_new(T_INT, p_int) : NULL;
}

static lex_tok_t* lexer_float(char* text)
{
    char* end;
    double* p_dbl = (double*)mem_allocate(sizeof(double), NULL);
    errno = 0;
    *p_dbl = strtod(text, &end);
    assert(errno == 0);
    return (end[0] == '\0') ? lex_tok_new(T_FLOAT, p_dbl) : NULL;
}

static lex_tok_t* lexer_bool(char* text)
{
    return lex_tok_new(T_BOOL, (void*)((intptr_t)((0 == strcmp(text,"true")) ? true : false)));
}

static lex_tok_t* lexer_var(char* text)
{
    return lex_tok_new(T_ID, lexer_dup(text));
}

static bool lexer_oneof(const char* class, char c) {
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

static bool is_float(char* text) {
    while (text[0] != '\0')
        if (text[0] == '.')
            return true;
        else
            text++;
    return false;
}

char* lexer_dup(const char* p_old) {
    size_t length = strlen(p_old);
    char* p_str = (char*)mem_allocate(length+1, NULL);
    memcpy(p_str, p_old, length);
    p_str[length] = '\0';
    return p_str;
}

static int read_radix(char ch) {
    int ret = -1;
    switch(ch) {
        case 'b': ret = 2;  break;
        case 'o': ret = 8;  break;
        case 'd': ret = 10; break;
        case 'h': ret = 16; break;
    }
    return ret;
}
