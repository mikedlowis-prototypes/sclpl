#include <sclpl.h>

static AST* ast(ASTType type)
{
    AST* tree = emalloc(sizeof(AST));
    memset(tree, 0, sizeof(AST));
    tree->type = type;
    return tree;
}

AST* String(Tok* val)
{
    AST* node = ast(AST_STRING);
    node->value.text = val->value.text;
    return node;
}

char* string_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_STRING);
    return val->value.text;
}

AST* Symbol(Tok* val)
{
    AST* node = ast(AST_SYMBOL);
    node->value.text = val->value.text;
    return node;
}

char* symbol_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_SYMBOL);
    return val->value.text;
}

AST* Char(Tok* val)
{
    AST* node = ast(AST_CHAR);
    node->value.character = val->value.character;
    return node;
}

uint32_t char_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_CHAR);
    return val->value.character;
}

AST* Integer(Tok* val)
{
    AST* node = ast(AST_INT);
    node->value.integer = val->value.integer;
    return node;
}

intptr_t integer_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_INT);
    return val->value.integer;
}

intptr_t temp_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_TEMP);
    return val->value.integer;
}

AST* Float(Tok* val)
{
    AST* node = ast(AST_FLOAT);
    node->value.floating = val->value.floating;
    return node;
}

double float_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_FLOAT);
    return val->value.floating;
}

AST* Bool(Tok* val)
{
    AST* node = ast(AST_BOOL);
    node->value.boolean = val->value.boolean;
    return node;
}

bool bool_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_BOOL);
    return val->value.boolean;
}

AST* Ident(Tok* val)
{
    AST* node = ast(AST_IDENT);
    node->value.text = val->value.text;
    return node;
}

char* ident_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_IDENT);
    return val->value.text;
}

AST* Def(Tok* name, AST* value)
{
    AST* node = ast(AST_DEF);
    node->value.def.name = name->value.text;
    node->value.def.value = value;
    return node;
}

char* def_name(AST* def)
{
    assert(def != NULL);
    assert(def->type == AST_DEF);
    return def->value.def.name;
}

AST* def_value(AST* def)
{
    assert(def != NULL);
    assert(def->type == AST_DEF);
    return def->value.def.value;
}

AST* IfExpr(void)
{
    return ast(AST_IF);
}

AST* ifexpr_cond(AST* ifexpr)
{
    return ifexpr->value.ifexpr.cond;
}

void ifexpr_set_cond(AST* ifexpr, AST* cond)
{
    ifexpr->value.ifexpr.cond = cond;
}

AST* ifexpr_then(AST* ifexpr)
{
    return ifexpr->value.ifexpr.bthen;
}

void ifexpr_set_then(AST* ifexpr, AST* bthen)
{
    ifexpr->value.ifexpr.bthen = bthen;
}

AST* ifexpr_else(AST* ifexpr)
{
    return ifexpr->value.ifexpr.belse;
}

void ifexpr_set_else(AST* ifexpr, AST* belse)
{
    ifexpr->value.ifexpr.belse = belse;
}

AST* Func(void)
{
    AST* node = ast(AST_FUNC);
    vec_init(&(node->value.func.args));
    node->value.func.body = NULL;
    return node;
}

vec_t* func_args(AST* func)
{
    return &(func->value.func.args);
}

AST* func_body(AST* func)
{
    return func->value.func.body;
}

void func_add_arg(AST* func, AST* arg)
{
    vec_push_back(func_args(func), arg);
}

void func_set_body(AST* func, AST* body)
{
    func->value.func.body = body;
}

AST* FnApp(AST* fnapp)
{
    AST* node = ast(AST_FNAPP);
    node->value.fnapp.fn = fnapp;
    vec_init(&(node->value.fnapp.args));
    return node;
}

void fnapp_set_fn(AST* fnapp, AST* fn)
{
    AST* old = fnapp->value.fnapp.fn;
    fnapp->value.fnapp.fn = fn;
}

AST* fnapp_fn(AST* fnapp)
{
    return fnapp->value.fnapp.fn;
}

vec_t* fnapp_args(AST* fnapp)
{
    return &(fnapp->value.fnapp.args);
}

void fnapp_add_arg(AST* fnapp, AST* arg)
{
    vec_push_back(&(fnapp->value.fnapp.args), arg);
}

