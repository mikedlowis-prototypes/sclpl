#include <sclpl.h>
#include <stdarg.h>

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
static AST* func_app(Parser* p, AST* fn);
static AST* token_to_tree(Parser* p, Tok* tok);

/* Parsing Routines
 *****************************************************************************/
static Tok* peek(Parser* p) {
    if (T_NONE == p->tok.type)
        gettoken(p, &(p->tok));
    return &(p->tok);
}

static void error(Parser* parser, const char* fmt, ...) {
    Tok* tok = peek(parser);
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "<file>:%zu:%zu: error: ", tok->line, tok->col);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
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
    Tok* id = expect_val(p, T_ID);
    Type* type = type_annotation(p);
    expect(p, T_ASSIGN);
    AST* expr = const_expression(p);
    sym_add(&(p->syms), (constant ? SF_CONSTANT : 0), id->value.text, type);
    if (!types_equal(type, expr->datatype))
        error(p, "type mismatch");
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

static Type* get_typedef(Parser* p, char* typename) {
   Sym* sym = sym_get(&(p->syms), typename);
    if (!sym) error(p, "unknown type '%s'", typename);
    return sym->type;
}

static Type* type_annotation(Parser* p) {
    Tok* id = expect_val(p, T_ID);
    return get_typedef(p, id->value.text);
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
            ret = token_to_tree(p, tok);
            tok->type = T_NONE;
            break;
        default:
            error(p, "not a valid literal");
    }
    return ret;
}

static AST* add_type(Parser* p, AST* ast, char* typename) {
    ast->datatype = get_typedef(p, typename);
    return ast;
}

static AST* token_to_tree(Parser* p, Tok* tok) {
    switch (tok->type) {
        case T_BOOL:   return add_type(p, Bool(tok), "bool");
        case T_CHAR:   return add_type(p, Char(tok), "char");
        case T_STRING: return add_type(p, String(tok), "string");
        case T_INT:    return add_type(p, Integer(tok), "int");
        case T_FLOAT:  return add_type(p, Float(tok), "float");
        case T_ID:     return add_type(p, Ident(tok), tok->value.text);
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
