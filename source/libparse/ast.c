#include <libparse.h>

AST* Require(char* name)
{
    (void)name;
    return NULL;
}

char* require_name(AST* req)
{
    (void)req;
    return NULL;
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

AST* String(char* val)
{
    (void)val;
    return NULL;
}

char* string_value(AST* val)
{
    (void)val;
    return NULL;
}

AST* Symbol(char* val)
{
    (void)val;
    return NULL;
}

char* symbol_value(AST* val)
{
    (void)val;
    return NULL;
}

AST* Char(uint32_t val)
{
    (void)val;
    return NULL;
}

uint32_t char_value(AST* val)
{
    (void)val;
    return 0;
}

AST* Integer(intptr_t val)
{
    (void)val;
    return NULL;
}

intptr_t integer_value(AST* val)
{
    (void)val;
    return 0;
}

AST* Float(double val)
{
    (void)val;
    return NULL;
}

double float_value(AST* val)
{
    (void)val;
    return 0.0;
}

AST* Bool(bool val)
{
    (void)val;
    return NULL;
}

bool bool_value(AST* val)
{
    (void)val;
    return false;
}

AST* Ident(char* val)
{
    (void)val;
    return NULL;
}

char ident_value(AST* val)
{
    (void)val;
    return 0;
}

