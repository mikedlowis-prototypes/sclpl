#include <sclpl.h>

static AST* const_definition(Parser* p, bool constant);
static AST* const_expression(Parser* p);
static AST* definition(Parser* p);
static AST* expression(Parser* p);
static AST* identifier(Parser* p);
static AST* function(Parser* p);
static Type* type_annotation(Parser* p);
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
        TokType type = peek(p)->type;
        if (accept(p, T_LET) || accept(p, T_VAR))
            ret = const_definition(p, (type == T_LET));
//        else if (accept(p, T_TYPE))
//            ret = type_definition(p);
//        else if (accept(p, T_FUN))
//            ret = func_definition(p);
        else
            error(p, "only definitions are allowed at the toplevel");
    }
    return ret;
}

static AST* const_definition(Parser* p, bool constant) {
    AST* expr;
    Tok* id = expect_val(p, T_ID);
    type_annotation(p);
    expect(p, T_ASSIGN);
    expr = const_expression(p);
    return Var(id, expr, constant);
}

static AST* const_expression(Parser* p) {
    AST* expr = NULL;
    if (accept(p, T_LPAR)) {
        expr = const_expression(p);
        expect(p, T_RPAR);
    } else if (match(p, T_ID)) {
        expr = identifier(p);
    } else {
        expr = literal(p);
    }
    return expr;
}

static Type* type_annotation(Parser* p) {
    expect(p, T_ID);
    return NULL;
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
            ret = token_to_tree(tok);
            tok->type = T_NONE;
            break;
        default:
            error(p, "Expected a literal");
    }
    return ret;
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

static AST* identifier(Parser* p) {
    Tok* tok = peek(p);
    if (tok->type == T_ID) {
        AST* ast = Ident(tok);
        tok->type = T_NONE;
        return ast;
    } else {
        error(p, "Expected an identifier");
        return NULL;
    }
}
