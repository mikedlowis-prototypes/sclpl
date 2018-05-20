/**
  @file parser.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <sclpl.h>

/* Private Declarations
 *****************************************************************************/
// Sentinel EOF Token
Tok tok_eof = { NULL, 0, 0, T_END_FILE, {0} };

// Grammar Routines
static AST* require(Parser* p);
static AST* definition(Parser* p);
static AST* expression(Parser* p);
static AST* if_stmnt(Parser* p);
static AST* function(Parser* p);
static AST* literal(Parser* p);
static AST* expr_block(Parser* p);
static AST* token_to_tree(Tok* tok);
static AST* func_app(Parser* p, AST* fn);
static void type_annotation(Parser* p);

// Parsing Routines
static void fetch(Parser* parser);
static Tok* peek(Parser* parser);
static bool parser_eof(Parser* parser);
static void parser_resume(Parser* parser);
static void error(Parser* parser, const char* text);
static bool match(Parser* parser, TokType type);
static Tok* accept(Parser* parser, TokType type);
static Tok* expect(Parser* parser, TokType type);

/* Grammar Definition
 *****************************************************************************/
AST* toplevel(Parser* p)
{
    AST* ret = NULL;
    if (!match(p, T_END_FILE)) {
        if (accept(p, T_REQUIRE))
            ret = require(p);
        else if (accept(p, T_DEF))
            ret = definition(p);
        else
            error(p, "expressions are not allowed at the toplevel");
            //ret = expression(p);
    }
    return ret;
}

static AST* definition(Parser* p)
{
    Tok* id = expect(p, T_ID);
    AST* expr;
    if (peek(p)->type == T_LPAR) {
        expr = function(p);
    } else {
        type_annotation(p);
        expr = expression(p);
        expect(p, T_END);
    }
    return Def(id, expr);
}

static AST* require(Parser* p)
{
    AST* ast = Require(expect(p, T_STRING));
    expect(p, T_END);
    return ast;
}

static AST* expression(Parser* p)
{
    AST* expr = NULL;
    if (accept(p, T_LPAR)) {
        expr = expression(p);
        expect(p, T_RPAR);
    } else if (accept(p, T_IF)) {
        expr = if_stmnt(p);
    } else if (accept(p, T_FN)) {
        expr = function(p);
    } else if (match(p, T_ID)) {
        expr = Ident(expect(p,T_ID));
    } else {
        expr = literal(p);
    }
    /* Check if this is a function application */
    if (peek(p)->type == T_LPAR) {
        expr = func_app(p, expr);
    }
    return expr;
}

static AST* if_stmnt(Parser* p)
{
    AST* ifexpr = IfExpr();
    ifexpr_set_cond( ifexpr, expression(p) );
    accept(p, T_THEN);
    ifexpr_set_then( ifexpr, expr_block(p) );
    if (accept(p, T_ELSE))
        ifexpr_set_else( ifexpr, expr_block(p) );
    expect(p,T_END);
    return ifexpr;
}

static AST* function(Parser* p)
{
    AST* func = Func();
    expect(p, T_LPAR);
    while(peek(p)->type != T_RPAR) {
        func_add_arg(func, Ident(expect(p,T_ID)));
        type_annotation(p);
        if(peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p, T_RPAR);
    type_annotation(p);
    func_set_body(func, expr_block(p));
    expect(p, T_END);
    return func;
}

static AST* literal(Parser* p)
{
    AST* ret = NULL;
    Tok* tok = peek(p);
    switch (tok->type) {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            ret = token_to_tree(expect(p, tok->type));
            break;
        default:
            error(p, "Expected a literal");
    }
    return ret;
}

static AST* expr_block(Parser* p)
{
    AST* block = NULL;
    vec_t exprs;
    vec_init(&exprs);
    /* Build all expressions into let forms with no bodies */
    do {
        if (accept(p, T_DEF)) {
            AST* def = definition(p);
            Tok name = { .value.text = def_name(def) };
            vec_push_back(&exprs, Let(Ident(&name), def_value(def), NULL));
        } else {
            vec_push_back(&exprs, Let(TempVar(), expression(p), NULL));
        }
    } while(!match(p, T_END) && !match(p, T_ELSE));
    /* Now nest all of the let forms making sure that the last one returns
     * it's definition as its body */
    for (int i = vec_size(&exprs); i > 0; i--) {
        AST* let = (AST*)vec_at(&exprs,i-1);
        let_set_body(let, (block == NULL) ? let_var(let) : block);
        block = let;
    }
    vec_deinit(&exprs);
    return block;
}

static AST* token_to_tree(Tok* tok)
{
    switch (tok->type) {
        case T_BOOL:   return Bool(tok);
        case T_CHAR:   return Char(tok);
        case T_STRING: return String(tok);
        case T_INT:    return Integer(tok);
        case T_FLOAT:  return Float(tok);
        case T_ID:     return Ident(tok);
        default:       return NULL;
    }
}

static AST* func_app(Parser* p, AST* fn)
{
    AST* app = FnApp(fn);
    expect(p,T_LPAR);
    while (peek(p)->type != T_RPAR) {
        fnapp_add_arg(app, expression(p));
        if (peek(p)->type != T_RPAR)
            expect(p, T_COMMA);
    }
    expect(p,T_RPAR);
    return app;
}

static void type_annotation(Parser* p)
{
    expect(p, T_ID);
    /* array type */
    if (accept(p,T_LBRACK)) {
        accept(p, T_INT);
        expect(p, T_RBRACK);
    /* reference type */
    } else if (accept(p, T_AMP)) {

    }
}

/* Parsing Routines
 *****************************************************************************/
Parser* parser_new(char* prompt, FILE* input)
{
    Parser* parser  = emalloc(sizeof(Parser));
    parser->line    = NULL;
    parser->index   = 0;
    parser->lineno  = 0;
    parser->input   = input;
    parser->prompt  = prompt;
    parser->tok     = NULL;
    return parser;
}

static void fetch(Parser* parser)
{
    parser->tok = gettoken(parser);
    if (NULL == parser->tok)
        parser->tok = &tok_eof;
}

static Tok* peek(Parser* parser)
{
    if (NULL == parser->tok)
        fetch(parser);
    return parser->tok;
}

static bool parser_eof(Parser* parser)
{
    return (peek(parser)->type == T_END_FILE);
}

static void parser_resume(Parser* parser)
{
    if ((NULL != parser->tok) && (&tok_eof != parser->tok))
        parser->tok = NULL;
    /* We ignore the rest of the current line and attempt to start parsing
     * again on the next line */
    fetchline(parser);
}

static void error(Parser* parser, const char* text)
{
    Tok* tok = peek(parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, text);
    exit(1);
}

static bool match(Parser* parser, TokType type)
{
    return (peek(parser)->type == type);
}

static Tok* accept(Parser* parser, TokType type)
{
    Tok* tok = peek(parser);
    if (tok->type == type) {
        parser->tok = NULL;
        return tok;
    }
    return NULL;
}

static Tok* expect(Parser* parser, TokType type)
{
    Tok* tok = accept(parser, type);
    if (tok == NULL)
        error(parser, "Unexpected token");
    return tok;
}

