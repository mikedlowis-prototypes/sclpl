/**
  @file lexer.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <libparse.h>

/* Token Scanning
 *****************************************************************************/
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

static void fetchline(Parser* ctx) {
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

void skipline(Parser* ctx)
{
    fetchline(ctx);
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
            fetchline(ctx);
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
        fetchline(ctx);
    /* Fast forward past whitespace and read a newline if necessary  */
    while(!eof(ctx)) {
        if ('\0' == current(ctx)) {
            fetchline(ctx);
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

/* Lexical Analysis
 *****************************************************************************/
static char* dupstring(const char* old) {
    size_t length = strlen(old);
    char* str = (char*)mem_allocate(length+1, NULL);
    memcpy(str, old, length);
    str[length] = '\0';
    return str;
}

static bool char_oneof(const char* class, char c) {
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

static void token_free(void* obj)
{
    Tok* tok = (Tok*)obj;
    if ((tok->type != T_BOOL) && (tok->type != T_CHAR) && (NULL != tok->value.text))
        mem_release(tok->value.text);
}

static Tok* token(TokType type, char* text)
{
    Tok* tok = (Tok*)mem_allocate(sizeof(Tok), &token_free);
    tok->type = type;
    tok->value.text = text;
    return tok;
}

static Tok* punctuation(char* text)
{
    Tok* tok = NULL;
    switch (text[0]) {
        case '(':  tok = token(T_LPAR, NULL);   break;
        case ')':  tok = token(T_RPAR, NULL);   break;
        case '{':  tok = token(T_LBRACE, NULL); break;
        case '}':  tok = token(T_RBRACE, NULL); break;
        case '[':  tok = token(T_LBRACK, NULL); break;
        case ']':  tok = token(T_RBRACK, NULL); break;
        case ';':  tok = token(T_END, NULL);    break;
        case ',':  tok = token(T_COMMA, NULL);  break;
        case '\'': tok = token(T_SQUOTE, NULL); break;
    }
    return tok;
}

static Tok* character(char* text)
{
    Tok* tok = NULL;
    static const char* lookuptable[5] = {
        " \0space",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab"
    };
    if (strlen(text) == 2) {
        tok = token(T_CHAR, (void*)((intptr_t)text[1]));
    } else {
        for(int i = 0; i < 5; i++) {
            if (0 == strcmp(&text[1], &(lookuptable[i][2]))) {
                tok = token(T_CHAR, (void*)((intptr_t)lookuptable[i][0]));
                break;
            }
        }
        if (NULL == tok)
            tok = token(T_ID, text);
    }
    return tok;
}

static Tok* integer(char* text, int base)
{
    char* end;
    long* integer = (long*)mem_allocate(sizeof(long), NULL);
    errno = 0;
    *integer = strtol(text, &end, base);
    assert(errno == 0);
    return NULL;
    //return (end[0] == '\0') ? token(T_INT, integer) : NULL;
}

static int getradix(char ch) {
    int ret = -1;
    switch(ch) {
        case 'b': ret = 2;  break;
        case 'o': ret = 8;  break;
        case 'd': ret = 10; break;
        case 'h': ret = 16; break;
    }
    return ret;
}

static Tok* radixint(char* text)
{
    Tok* ret = integer(&text[2], getradix(text[1]));
    if (NULL == ret)
        ret = token(T_ID, text);
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

static Tok* floating(char* text)
{
    char* end;
    double* dbl = (double*)mem_allocate(sizeof(double), NULL);
    errno = 0;
    *dbl = strtod(text, &end);
    assert(errno == 0);
    //return (end[0] == '\0') ? token(T_FLOAT, dbl) : NULL;
    return NULL;
}

static Tok* number(char* text)
{
    Tok* tok = NULL;
    if (is_float(text))
        tok = floating(text);
    else
        tok = integer(text, 10);
    return (NULL == tok) ? token(T_ID, text) : tok;
}

static Tok* boolean(char* text)
{
    //return token(T_BOOL, (void*)((intptr_t)((0 == strcmp(text,"true")) ? true : false)));
    return NULL;
}

static Tok* classify(const char* file, size_t line, size_t col, char* text)
{
    Tok* tok = NULL;
    if (0 == strcmp(text,"end")) {
        tok = token(T_END, NULL);
    } else if (char_oneof("()[]{};,'", text[0])) {
        tok = punctuation(text);
    } else if ('"' == text[0]) {
        text[strlen(text)-1] = '\0';
        tok = token(T_STRING, dupstring(&text[1]));
    } else if (text[0] == '\\') {
        tok = character(text);
    } else if ((text[0] == '0') && char_oneof("bodh",text[1])) {
        tok = radixint(text);
    } else if (char_oneof("+-0123456789",text[0])) {
        tok = number(text);
    } else if ((0 == strcmp(text,"true")) || (0 == strcmp(text,"false"))) {
        tok = boolean(text);
    } else {
        tok = token(T_ID, text);
    }
    /* If we found a valid token then fill in the location details */
    if (NULL != tok) {
        tok->line = line;
        tok->col  = col;
    }
    return tok;
}

Tok* gettoken(Parser* ctx)
{
    Tok* tok = NULL;
    size_t line, col;
    char* text = scan(ctx, &line, &col);
    tok = classify(NULL, line, col, text);
    return tok;
}

