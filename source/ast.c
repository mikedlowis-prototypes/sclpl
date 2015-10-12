#include <sclpl.h>

static void ast_free(void* ptr)
{
}

static AST* ast(ASTType type)
{
    AST* tree = gc_alloc(sizeof(AST), &ast_free);
    memset(tree, 0, sizeof(AST));
    tree->type = type;
    return tree;
}

AST* String(char* val)
{
    AST* node = ast(AST_STRING);
    node->value.text = val;
    return node;
}

char* string_value(AST* val)
{
    assert(val->type == AST_STRING);
    return val->value.text;
}

AST* Symbol(char* val)
{
    AST* node = ast(AST_SYMBOL);
    node->value.text = val;
    return node;
}

char* symbol_value(AST* val)
{
    assert(val->type == AST_SYMBOL);
    return val->value.text;
}

AST* Char(uint32_t val)
{
    AST* node = ast(AST_CHAR);
    node->value.character = val;
    return node;
}

uint32_t char_value(AST* val)
{
    assert(val->type == AST_CHAR);
    return val->value.character;
}

AST* Integer(intptr_t val)
{
    AST* node = ast(AST_INT);
    node->value.integer = val;
    return node;
}

intptr_t integer_value(AST* val)
{
    assert(val->type == AST_INT);
    return val->value.integer;
}

AST* Float(double val)
{
    AST* node = ast(AST_FLOAT);
    node->value.floating = val;
    return node;
}

double float_value(AST* val)
{
    assert(val->type == AST_FLOAT);
    return val->value.floating;
}

AST* Bool(bool val)
{
    AST* node = ast(AST_BOOL);
    node->value.boolean = val;
    return node;
}

bool bool_value(AST* val)
{
    assert(val->type == AST_BOOL);
    return val->value.boolean;
}

AST* Ident(char* val)
{
    AST* node = ast(AST_IDENT);
    node->value.text = val;
    return node;
}

char* ident_value(AST* val)
{
    assert(val->type == AST_IDENT);
    return val->value.text;
}

AST* Require(char* name)
{
    AST* node = ast(AST_REQ);
    node->value.text = name;
    return node;
}

char* require_name(AST* req)
{
    assert(req->type == AST_REQ);
    return req->value.text;
}
























AST* Def(char* name, AST* value)
{
    (void)name;
    (void)value;
    return NULL;
}

char* def_name(AST* def)
{
    (void)def;
    return NULL;
}

AST* def_value(AST* def)
{
    (void)def;
    return NULL;
}

AST* Ann(char* name, AST* value)
{
    (void)name;
    (void)value;
    return NULL;
}

char* ann_name(AST* ann)
{
    (void)ann;
    return NULL;
}

AST* ann_value(AST* ann)
{
    (void)ann;
    return NULL;
}

AST* IfExpr(AST* cond, AST* bthen, AST* belse)
{
    (void)cond;
    (void)bthen;
    (void)belse;
    return NULL;
}

AST* ifexpr_condition(AST* ifexpr)
{
    (void)ifexpr;
    return NULL;
}

AST* ifexpr_branch_then(AST* ifexpr)
{
    (void)ifexpr;
    return NULL;
}

AST* ifexpr_branch_else(AST* ifexpr)
{
    (void)ifexpr;
    return NULL;
}

AST* Func(AST* args, AST* body)
{
    (void)args;
    (void)body;
    return NULL;
}

AST* func_args(AST* func)
{
    (void)func;
    return NULL;
}

AST* func_body(AST* func)
{
    (void)func;
    return NULL;
}

AST* Block(void)
{
    return NULL;
}

void block_append(AST* expr)
{
    (void)expr;
}

size_t block_size(AST* block)
{
    (void)block;
    return 0;
}

AST* block_get(size_t index)
{
    (void)index;
    return NULL;
}

