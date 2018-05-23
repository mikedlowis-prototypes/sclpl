#include <sclpl.h>

/* Private Declarations
 *****************************************************************************/
// Grammar Routines
static AST* const_definition(Parser* p);
static AST* const_expression(Parser* p);
static AST* definition(Parser* p);
static AST* expression(Parser* p);
static AST* function(Parser* p);
static void type_annotation(Parser* p);
static AST* literal(Parser* p);
static AST* expr_block(Parser* p);
static AST* if_stmnt(Parser* p);
static AST* token_to_tree(Tok* tok);
static AST* func_app(Parser* p, AST* fn);

/* Parsing Routines
 *****************************************************************************/
static Tok* peek(Parser* p) {
    if (T_NONE == p->tok.type)
        gettoken(p, &(p->tok));
    return &(p->tok);
}

static void error(Parser* parser, const char* text) {
    Tok* tok = peek(parser);
    fprintf(stderr, "<file>:%zu:%zu:Error: %s\n", tok->line, tok->col, text);
    exit(1);
}

static bool match(Parser* parser, TokType type) {
    return (peek(parser)->type == type);
}

static bool accept(Parser* parser, TokType type) {
    if (peek(parser)->type == type) {
        parser->tok.type = T_NONE;
        return true;
    }
    return false;
}

static void expect(Parser* parser, TokType type) {
    if (!accept(parser, type))
        error(parser, "Unexpected token");
}

static Tok* expect_val(Parser* parser, TokType type) {
    Tok* tok = NULL;
    if (peek(parser)->type == type) {
        tok = calloc(1, sizeof(Tok));
        *tok = *(peek(parser));
        parser->tok.type = T_NONE;
    } else {
        error(parser, "Unexpected token");
    }
    return tok;
}


/* Grammar Definition
 *****************************************************************************/
AST* toplevel(Parser* p) {
    AST* ret = NULL;
    if (!match(p, T_END_FILE)) {
        if (accept(p, T_LET))
            ret = const_definition(p);
        else
            error(p, "only definitions are allowed at the toplevel");
    }
    return ret;
}

static AST* const_definition(Parser* p) {
    AST* expr;
    Tok* id = expect_val(p, T_ID);
    if (peek(p)->type == T_LPAR) {
        expr = function(p);
    } else {
        type_annotation(p);
        expr = const_expression(p);
        expect(p, T_END);
    }
    return Def(id, expr);
}

static AST* const_expression(Parser* p) {
    AST* expr = NULL;
    if (accept(p, T_LPAR)) {
        expr = const_expression(p);
        expect(p, T_RPAR);
    } else if (accept(p, T_FN)) {
        expr = function(p);
    } else if (match(p, T_ID)) {
        expr = Ident(expect_val(p, T_ID));
    } else {
        expr = literal(p);
    }
    return expr;
}

static AST* definition(Parser* p) {
    AST* expr;
    Tok* id = expect_val(p, T_ID);
    if (peek(p)->type == T_LPAR) {
        expr = function(p);
    } else {
        type_annotation(p);
        expr = expression(p);
        expect(p, T_END);
    }
    return Def(id, expr);
}

static AST* expression(Parser* p) {
    AST* expr = NULL;
    if (accept(p, T_LPAR)) {
        expr = expression(p);
        expect(p, T_RPAR);
    } else if (accept(p, T_IF)) {
        expr = if_stmnt(p);
    } else if (accept(p, T_FN)) {
        expr = function(p);
    } else if (match(p, T_ID)) {
        expr = Ident(expect_val(p,T_ID));
    } else {
        expr = literal(p);
    }
    /* Check if this is a function application */
    if (peek(p)->type == T_LPAR)
        expr = func_app(p, expr);
    return expr;
}

static AST* function(Parser* p) {
    AST* func = Func();
    expect(p, T_LPAR);
    while(peek(p)->type != T_RPAR) {
        func_add_arg(func, Ident(expect_val(p,T_ID)));
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

static void type_annotation(Parser* p) {
    expect(p, T_ID);
    /* array type */
    if (accept(p,T_LBRACK)) {
        accept(p, T_INT);
        expect(p, T_RBRACK);
    /* reference type */
    } else if (accept(p, T_AMP)) {
        // TODO: implement reference types
    }
}

static AST* literal(Parser* p) {
    AST* ret = NULL;
    Tok* tok = peek(p);
    switch (tok->type) {
        case T_BOOL:
        case T_CHAR:
        case T_STRING:
        case T_INT:
        case T_FLOAT:
            ret = token_to_tree(expect_val(p, tok->type));
            break;
        default:
            error(p, "Expected a literal");
    }
    return ret;
}

static AST* expr_block(Parser* p) {
    AST* block = NULL;
    vec_t exprs;
    vec_init(&exprs);
    /* Build all expressions into let forms with no bodies */
    do {
        if (accept(p, T_LET)) {
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

static AST* token_to_tree(Tok* tok) {
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

static AST* if_stmnt(Parser* p) {
    AST* ifexpr = IfExpr();
    ifexpr_set_cond( ifexpr, expression(p) );
    accept(p, T_THEN);
    ifexpr_set_then( ifexpr, expr_block(p) );
    if (accept(p, T_ELSE))
        ifexpr_set_else( ifexpr, expr_block(p) );
    expect(p,T_END);
    return ifexpr;
}

static AST* func_app(Parser* p, AST* fn) {
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
