/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <libparse.h>

static char* read(Lexer* ctx, size_t* line, size_t* col);
static bool eof(Lexer* ctx);
static bool eol(Lexer* ctx);
static void getline(Lexer* ctx);
static void skip_ws(Lexer* ctx);
static char current(Lexer* ctx);
static bool oneof(Lexer* ctx, const char* p_set);
static char* dup(Lexer* ctx, size_t start_idx, size_t len);
static char* read_string(Lexer* ctx);

static Token* lexer_make_token(size_t line, size_t col, char* text);
static Token* lexer_punc(char* text);
static Token* lexer_char(char* text);
static Token* lexer_radix_int(char* text);
static Token* lexer_number(char* text);
static Token* lexer_integer(char* text, int base);
static Token* lexer_float(char* text);
static Token* lexer_bool(char* text);
static Token* lexer_var(char* text);
static bool lexer_oneof(const char* class, char c);
static bool is_float(char* text);
static int read_radix(char ch);

static void lex_tok_free(void* p_obj) {
    Token* p_tok = (Token*)p_obj;
    if ((p_tok->type != T_BOOL) && (p_tok->type != T_CHAR) && (NULL != p_tok->value))
        mem_release(p_tok->value);
}

Token* lex_tok_new(TokenType type, void* val) {
    Token* p_tok = (Token*)mem_allocate(sizeof(Token), &lex_tok_free);
    p_tok->type  = type;
    p_tok->value = val;
    return p_tok;
}

static void lexer_free(void* p_obj) {
}

Lexer* lexer_new(char* p_prompt, FILE* p_input) {
    Lexer* p_lexer = (Lexer*)mem_allocate(sizeof(Lexer), &lexer_free);

    p_lexer->p_line   = NULL;
    p_lexer->index    = 0;
    p_lexer->lineno   = 0;
    p_lexer->p_input  = p_input;
    p_lexer->p_prompt = p_prompt;

    return p_lexer;
}

Token* lexer_read(Lexer* p_lexer) {
    Token* p_tok = NULL;
    size_t line;
    size_t col;
    char* text = read(p_lexer, &line, &col);
    if (NULL != text) {
        p_tok = lexer_make_token(line, col, text);
        free(text);
    }
    return p_tok;
}

void lexer_skipline(Lexer* p_lexer) {
    getline(p_lexer);
}

static Token* lexer_make_token(size_t line, size_t col, char* text) {
    Token* p_tok = NULL;
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

static Token* lexer_punc(char* text)
{
    Token* p_tok = NULL;
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

static Token* lexer_char(char* text)
{
    Token* p_tok = NULL;
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

static Token* lexer_radix_int(char* text)
{
    Token* ret = lexer_integer(&text[2], read_radix(text[1]));
    if (NULL == ret)
        ret = lexer_var(text);
    return ret;
}

static Token* lexer_number(char* text)
{
    Token* p_tok = NULL;
    if (is_float(text))
        p_tok = lexer_float(text);
    else
        p_tok = lexer_integer(text, 10);
    return (NULL == p_tok) ? lexer_var(text) : p_tok;
}

static Token* lexer_integer(char* text, int base)
{
    char* end;
    long* p_int = (long*)mem_allocate(sizeof(long), NULL);
    errno = 0;
    *p_int = strtol(text, &end, base);
    assert(errno == 0);
    return (end[0] == '\0') ? lex_tok_new(T_INT, p_int) : NULL;
}

static Token* lexer_float(char* text)
{
    char* end;
    double* p_dbl = (double*)mem_allocate(sizeof(double), NULL);
    errno = 0;
    *p_dbl = strtod(text, &end);
    assert(errno == 0);
    return (end[0] == '\0') ? lex_tok_new(T_FLOAT, p_dbl) : NULL;
}

static Token* lexer_bool(char* text)
{
    return lex_tok_new(T_BOOL, (void*)((intptr_t)((0 == strcmp(text,"true")) ? true : false)));
}

static Token* lexer_var(char* text)
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

/*****************************************************************************/

char* read(Lexer* ctx, size_t* line, size_t* column) {
    char* p_tok = NULL;
    skip_ws(ctx);
    *line   = ctx->lineno;
    *column = ctx->index+1;
    if (!eof(ctx)) {
        if (oneof(ctx, "()[]{};,'")) {
            p_tok = dup(ctx, ctx->index, 1);
            ctx->index++;
        } else if (current(ctx) == '"') {
            p_tok = read_string(ctx);
        } else {
            size_t start = ctx->index;
            while(!oneof(ctx," \t\r\n()[]{};,'\"") &&
                  (current(ctx) != '\0')) {
                ctx->index++;
            }
            p_tok = dup(ctx, start, ctx->index - start);
        }
    }
    return p_tok;
}

static char* read_string(Lexer* ctx) {
    size_t capacity = 8;
    size_t index = 0;
    char*  tok = (char*)malloc(capacity);

    /* Skip the first " */
    tok[index++] = current(ctx);
    tok[index] = '\0';
    ctx->index++;

    /* Read the contents of the string */
    while ('"' != current(ctx)) {
        /* Resize the buffer if necessary */
        if ((index+2) >= capacity) {
            capacity = capacity << 1;
            tok = (char*)realloc(tok, capacity);
        }

        /* EOF results in an assertion (don't do) */
        if (eof(ctx))
            assert(false);

        /* Read the char */
        tok[index++] = current(ctx);
        tok[index] = '\0';
        ctx->index++;

        /* Get the next line if necessary */
        if ('\n' == tok[index-1])
            getline(ctx);
    }

    /* Skip the last " */
    tok[index++] = current(ctx);
    tok[index] = '\0';
    ctx->index++;

    return tok;
}

bool eof(Lexer* ctx)
{
    return (eol(ctx) && feof(ctx->p_input));
}

bool eol(Lexer* ctx)
{
    bool ret = true;
    size_t index = ctx->index;
    char ch;
    while((NULL != ctx->p_line) && ('\0' != (ch = ctx->p_line[index]))) {
        if((' '!=ch) && ('\t'!=ch) && ('\r'!=ch) && ('\n'!=ch)) {
            ret = false;
            break;
        }
        index++;
    }
    return ret;
}

void getline(Lexer* ctx) {
    int c;
    size_t capacity = 8;
    size_t index    = 0;
    /* Reallocate and clear the line buffer */
    ctx->p_line = realloc(ctx->p_line, capacity);
    ctx->p_line[0] = '\0';
    ctx->index = 0;

    /* If we have not yet reached the end of the file, read the next line */
    if (!eof(ctx)) {
        if (NULL != ctx->p_prompt)
            printf("%s", ctx->p_prompt);
        while(('\n' != (c = fgetc(ctx->p_input))) && (EOF != c)) {
            if (index+2 == capacity) {
                capacity <<= 1u;
                ctx->p_line = realloc(ctx->p_line, capacity);
            }
            ctx->p_line[index++] = c;
        }
        ctx->p_line[index++] = (c == EOF) ? '\0' : c;
        ctx->p_line[index++] = '\0';
        ctx->index = 0;
        /* Increment line count */
        ctx->lineno++;
    }
}

static void skip_ws(Lexer* ctx) {
    /* If we haven't read a line yet, read one now */
    if (NULL == ctx->p_line)
        getline(ctx);
    /* Fast forward past whitespace and read a newline if necessary  */
    while(!eof(ctx)) {
        if ('\0' == current(ctx)) {
            getline(ctx);
        } else if (oneof(ctx, " \t\r\n")) {
            ctx->index++;
        } else {
            break;
        }
    }
}

static char current(Lexer* ctx) {
    return ctx->p_line[ctx->index];
}

static bool oneof(Lexer* ctx, const char* p_set) {
    bool ret = false;
    size_t sz = strlen(p_set);
    for (size_t idx = 0; idx < sz; idx++) {
        if (current(ctx) == p_set[idx]) {
            ret = true;
            break;
        }
    }
    return ret;
}

static char* dup(Lexer* ctx, size_t start_idx, size_t len) {
    char* p_str = (char*)malloc(len+1);
    memcpy(p_str, &(ctx->p_line[start_idx]), len);
    p_str[len] = '\0';
    return p_str;
}


