#include <sclpl.h>

static void ast_free(void* ptr)
{
    AST* ast = (AST*)ptr;
    switch(ast->type) {
        case AST_IDENT:
        case AST_STRING:
            gc_delref(ast->value.text);
            break;

        case AST_REQ:
            gc_delref(ast->value.req.name);
            break;

        case AST_DEF:
            gc_delref(ast->value.def.name);
            gc_delref(ast->value.def.value);
            break;

        case AST_ANN:
            gc_delref(ast->value.ann.name);
            gc_delref(ast->value.ann.value);
            break;

        case AST_IF:
            gc_delref(ast->value.ifexpr.cond);
            gc_delref(ast->value.ifexpr.bthen);
            gc_delref(ast->value.ifexpr.belse);
            break;

        case AST_FUNC:
            //vec_deinit(&(ast->value.func.args));
            gc_delref(ast->value.func.body);
            break;

        default:
            break;
    }
}

static AST* ast(ASTType type)
{
    AST* tree = gc_alloc(sizeof(AST), &ast_free);
    memset(tree, 0, sizeof(AST));
    tree->type = type;
    return tree;
}

AST* String(Tok* val)
{
    AST* node = ast(AST_STRING);
    node->value.text = (char*)gc_addref(val->value.text);
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
    node->value.text = (char*)gc_addref(val->value.text);
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
    node->value.text = (char*)gc_addref(val->value.text);
    return node;
}

char* ident_value(AST* val)
{
    assert(val != NULL);
    assert(val->type == AST_IDENT);
    return val->value.text;
}

AST* Require(Tok* name)
{
    AST* node = ast(AST_REQ);
    node->value.text = (char*)gc_addref(name->value.text);
    return node;
}

char* require_name(AST* req)
{
    assert(req != NULL);
    assert(req->type == AST_REQ);
    return req->value.text;
}

AST* Def(Tok* name, AST* value)
{
    AST* node = ast(AST_DEF);
    node->value.def.name = (char*)gc_addref(name->value.text);
    node->value.def.value = (AST*)gc_addref(value);
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
    ifexpr->value.ifexpr.cond = (AST*)gc_addref(cond);
}

AST* ifexpr_then(AST* ifexpr)
{
    return ifexpr->value.ifexpr.bthen;
}

void ifexpr_set_then(AST* ifexpr, AST* bthen)
{
    ifexpr->value.ifexpr.bthen = (AST*)gc_addref(bthen);
}

AST* ifexpr_else(AST* ifexpr)
{
    return ifexpr->value.ifexpr.belse;
}

void ifexpr_set_else(AST* ifexpr, AST* belse)
{
    ifexpr->value.ifexpr.belse = (AST*)gc_addref(belse);
}

AST* Block(void)
{
    AST* node = ast(AST_BLOCK);
    vec_init(&(node->value.exprs));
    return node;
}

void block_append(AST* block, AST* expr)
{
    vec_push_back(&(block->value.exprs), expr);
}

size_t block_size(AST* block)
{
    return vec_size(&(block->value.exprs));
}

AST* block_get(AST* block, size_t index)
{
    return (AST*)vec_at(&(block->value.exprs), index);
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


//AST* Ann(char* name, AST* value)
//{
//    (void)name;
//    (void)value;
//    return NULL;
//}
//
//char* ann_name(AST* ann)
//{
//    (void)ann;
//    return NULL;
//}
//
//AST* ann_value(AST* ann)
//{
//    (void)ann;
//    return NULL;
//}
//
//AST* Block(void)
//{
//    return NULL;
//}
//
//void block_append(AST* expr)
//{
//    (void)expr;
//}
//
//size_t block_size(AST* block)
//{
//    (void)block;
//    return 0;
//}
//
//AST* block_get(size_t index)
//{
//    (void)index;
//    return NULL;
//}

