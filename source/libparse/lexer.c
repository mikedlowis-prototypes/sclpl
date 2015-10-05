/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <libparse.h>

static char current(Parser* ctx) {
    return ctx->line[ctx->index];
}

static bool eol(Parser* ctx)
{
    bool ret = true;
    size_t index = ctx->index;
    char ch;
    while((NULL != ctx->line) && ('\0' != (ch = ctx->line[index]))) {
        if((' '!=ch) && ('\t'!=ch) && ('\r'!=ch) && ('\n'!=ch)) {
            ret = false;
            break;
        }
        index++;
    }
    return ret;
}

static bool eof(Parser* ctx)
{
    return (eol(ctx) && feof(ctx->input));
}

static bool oneof(Parser* ctx, const char* set) {
    bool ret = false;
    size_t sz = strlen(set);
    for (size_t idx = 0; idx < sz; idx++) {
        if (current(ctx) == set[idx]) {
            ret = true;
            break;
        }
    }
    return ret;
}

static char* dup(Parser* ctx, size_t start_idx, size_t len) {
    char* str = (char*)malloc(len+1);
    memcpy(str, &(ctx->line[start_idx]), len);
    str[len] = '\0';
    return str;
}

static void getline(Parser* ctx) {
    int c;
    size_t capacity = 8;
    size_t index    = 0;
    /* Reallocate and clear the line buffer */
    ctx->line = realloc(ctx->line, capacity);
    ctx->line[0] = '\0';
    ctx->index = 0;

    /* If we have not yet reached the end of the file, read the next line */
    if (!eof(ctx)) {
        if (NULL != ctx->prompt)
            printf("%s", ctx->prompt);
        while(('\n' != (c = fgetc(ctx->input))) && (EOF != c)) {
            if (index+2 == capacity) {
                capacity <<= 1u;
                ctx->line = realloc(ctx->line, capacity);
            }
            ctx->line[index++] = c;
        }
        ctx->line[index++] = (c == EOF) ? '\0' : c;
        ctx->line[index++] = '\0';
        ctx->index = 0;
        /* Increment line count */
        ctx->lineno++;
    }
}

static char* read_string(Parser* ctx) {
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

static void skip_ws(Parser* ctx) {
    /* If we haven't read a line yet, read one now */
    if (NULL == ctx->line)
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

static char* scan(Parser* ctx, size_t* line, size_t* column) {
    char* tok = NULL;
    skip_ws(ctx);
    *line   = ctx->lineno;
    *column = ctx->index+1;
    if (!eof(ctx)) {
        if (oneof(ctx, "()[]{};,'")) {
            tok = dup(ctx, ctx->index, 1);
            ctx->index++;
        } else if (current(ctx) == '"') {
            tok = read_string(ctx);
        } else {
            size_t start = ctx->index;
            while(!oneof(ctx," \t\r\n()[]{};,'\"") && (current(ctx) != '\0')) {
                ctx->index++;
            }
            tok = dup(ctx, start, ctx->index - start);
        }
    }
    return tok;
}





#if 0

// Scanning phase
static char* scan(Parser* ctx, size_t* line, size_t* col);

// Classification phase
Tok* classify(const char* file, size_t line, size_t col, char* text);

Tok* gettoken(Parser* lexer) {
    Token* tok = NULL;
    size_t line, col;
    char* text = scan(lexer, &line, &col);
    tok = classify(NULL, line, col, text);
    return tok;
}

#endif


void skipline(Parser* ctx)
{
}

Tok* gettoken(Parser* ctx)
{
    Tok* tok = NULL;
    size_t line, col;
    char* text = scan(lexer, &line, &col);
    tok = classify(NULL, line, col, text);
    return tok;
}






//static char* read(Parser* ctx, size_t* line, size_t* col);
//static bool eof(Parser* ctx);
//static bool eol(Parser* ctx);
//static void getline(Parser* ctx);
//static void skip_ws(Parser* ctx);
//static char current(Parser* ctx);
//static bool oneof(Parser* ctx, const char* set);
//static char* dup(Parser* ctx, size_t start_idx, size_t len);
//static char* read_string(Parser* ctx);
//
//static Token* lexer_make_token(size_t line, size_t col, char* text);
//static Token* lexer_punc(char* text);
//static Token* lexer_char(char* text);
//static Token* lexer_radix_int(char* text);
//static Token* lexer_number(char* text);
//static Token* lexer_integer(char* text, int base);
//static Token* lexer_float(char* text);
//static Token* lexer_bool(char* text);
//static Token* lexer_var(char* text);
//static bool lexer_oneof(const char* class, char c);
//static bool is_float(char* text);
//static int read_radix(char ch);
//
//static void lex_tok_free(void* obj) {
//    Token* tok = (Token*)obj;
//    if ((tok->type != T_BOOL) && (tok->type != T_CHAR) && (NULL != tok->value))
//        mem_release(tok->value);
//}
//
//Token* token(TokenType type, void* val) {
//    Token* tok = (Token*)mem_allocate(sizeof(Token), &lex_tok_free);
//    tok->type  = type;
//    tok->value = val;
//    return tok;
//}
//
//static void lexer_free(void* obj) {
//}
//
//Parser* lexer_new(char* prompt, FILE* input) {
//    Parser* lexer = (Parser*)mem_allocate(sizeof(Parser), &lexer_free);
//
//    lexer->line   = NULL;
//    lexer->index    = 0;
//    lexer->lineno   = 0;
//    lexer->input  = input;
//    lexer->prompt = prompt;
//
//    return lexer;
//}
//
//Token* lexer_read(Parser* lexer) {
//    Token* tok = NULL;
//    size_t line, col;
//    char* text = read(lexer, &line, &col);
//    if (NULL != text) {
//        tok = lexer_make_token(line, col, text);
//        free(text);
//    }
//    return tok;
//}
//
//void lexer_skipline(Parser* lexer) {
//    getline(lexer);
//}
//
//static Token* lexer_make_token(size_t line, size_t col, char* text) {
//    Token* tok = NULL;
//    if (0 == strcmp(text,"end")) {
//        tok = token(T_END, NULL);
//    } else if (lexer_oneof("()[]{};,'", text[0])) {
//        tok = lexer_punc(text);
//    } else if ('"' == text[0]) {
//        text[strlen(text)-1] = '\0';
//        tok = token(T_STRING, lexer_dup(&text[1]));
//    } else if (text[0] == '\\') {
//        tok = lexer_char(text);
//    } else if ((text[0] == '0') && lexer_oneof("bodh",text[1])) {
//        tok = lexer_radix_int(text);
//    } else if (lexer_oneof("+-0123456789",text[0])) {
//        tok = lexer_number(text);
//    } else if ((0 == strcmp(text,"true")) || (0 == strcmp(text,"false"))) {
//        tok = lexer_bool(text);
//    } else {
//        tok = lexer_var(text);
//    }
//    /* If we found a valid token then fill in the location details */
//    if (NULL != tok) {
//        tok->line = line;
//        tok->col  = col;
//    }
//    return tok;
//}
//
//static Token* lexer_punc(char* text)
//{
//    Token* tok = NULL;
//    switch (text[0]) {
//        case '(':  tok = token(T_LPAR,   NULL); break;
//        case ')':  tok = token(T_RPAR,   NULL); break;
//        case '{':  tok = token(T_LBRACE, NULL); break;
//        case '}':  tok = token(T_RBRACE, NULL); break;
//        case '[':  tok = token(T_LBRACK, NULL); break;
//        case ']':  tok = token(T_RBRACK, NULL); break;
//        case ';':  tok = token(T_END,    NULL); break;
//        case ',':  tok = token(T_COMMA,  NULL); break;
//        case '\'': tok = token(T_SQUOTE, NULL); break;
//    }
//    return tok;
//}
//
//static Token* lexer_char(char* text)
//{
//    Token* tok = NULL;
//    static const char* lookutable[5] = {
//        " \0space",
//        "\n\0newline",
//        "\r\0return",
//        "\t\0tab",
//        "\v\0vtab"
//    };
//    if (strlen(text) == 2) {
//        tok = token(T_CHAR, (void*)((intptr_t)text[1]));
//    } else {
//        for(int i = 0; i < 5; i++) {
//            if (0 == strcmp(&text[1], &(lookutable[i][2]))) {
//                tok = token(T_CHAR, (void*)((intptr_t)lookutable[i][0]));
//                break;
//            }
//        }
//        if (NULL == tok)
//            tok = lexer_var(text);
//    }
//    return tok;
//}
//
//static Token* lexer_radix_int(char* text)
//{
//    Token* ret = lexer_integer(&text[2], read_radix(text[1]));
//    if (NULL == ret)
//        ret = lexer_var(text);
//    return ret;
//}
//
//static Token* lexer_number(char* text)
//{
//    Token* tok = NULL;
//    if (is_float(text))
//        tok = lexer_float(text);
//    else
//        tok = lexer_integer(text, 10);
//    return (NULL == tok) ? lexer_var(text) : tok;
//}
//
//static Token* lexer_integer(char* text, int base)
//{
//    char* end;
//    long* int = (long*)mem_allocate(sizeof(long), NULL);
//    errno = 0;
//    *int = strtol(text, &end, base);
//    assert(errno == 0);
//    return (end[0] == '\0') ? token(T_INT, int) : NULL;
//}
//
//static Token* lexer_float(char* text)
//{
//    char* end;
//    double* dbl = (double*)mem_allocate(sizeof(double), NULL);
//    errno = 0;
//    *dbl = strtod(text, &end);
//    assert(errno == 0);
//    return (end[0] == '\0') ? token(T_FLOAT, dbl) : NULL;
//}
//
//static Token* lexer_bool(char* text)
//{
//    return token(T_BOOL, (void*)((intptr_t)((0 == strcmp(text,"true")) ? true : false)));
//}
//
//static Token* lexer_var(char* text)
//{
//    return token(T_ID, lexer_dup(text));
//}
//
//static bool lexer_oneof(const char* class, char c) {
//    bool ret = false;
//    size_t sz = strlen(class);
//    for (size_t idx = 0; idx < sz; idx++) {
//        if (c == class[idx]) {
//            ret = true;
//            break;
//        }
//    }
//    return ret;
//}
//
//static bool is_float(char* text) {
//    while (text[0] != '\0')
//        if (text[0] == '.')
//            return true;
//        else
//            text++;
//    return false;
//}
//
//char* lexer_dup(const char* old) {
//    size_t length = strlen(old);
//    char* str = (char*)mem_allocate(length+1, NULL);
//    memcpy(str, old, length);
//    str[length] = '\0';
//    return str;
//}
//
//static int read_radix(char ch) {
//    int ret = -1;
//    switch(ch) {
//        case 'b': ret = 2;  break;
//        case 'o': ret = 8;  break;
//        case 'd': ret = 10; break;
//        case 'h': ret = 16; break;
//    }
//    return ret;
//}
//
///*****************************************************************************/
//

