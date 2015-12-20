#include <sclpl.h>

static void ast_free(void* ptr)
{
//    AST_LET AST_TEMP

    AST* ast = (AST*)ptr;
    switch(ast->type) {
        case AST_REQ:
        case AST_IDENT:
        case AST_STRING:
        case AST_SYMBOL:
            gc_delref(ast->value.text);
            break;

        case AST_DEF:
            gc_delref(ast->value.def.name);
            gc_delref(ast->value.def.value);
            break;

        case AST_IF:
            gc_delref(ast->value.ifexpr.cond);
            gc_delref(ast->value.ifexpr.bthen);
            gc_delref(ast->value.ifexpr.belse);
            break;

        case AST_FUNC:
            vec_deinit(&(ast->value.func.args));
            gc_delref(ast->value.func.body);
            break;

        case AST_FNAPP:
            gc_delref(ast->value.fnapp.fn);
            vec_deinit(&(ast->value.fnapp.args));
            break;

        case AST_BLOCK:
            vec_deinit(&(ast->value.exprs));
            break;

        case AST_LET:
            break;

        case AST_TEMP:
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
    vec_push_back(&(block->value.exprs), gc_addref(expr));
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
    vec_push_back(func_args(func), gc_addref(arg));
}

void func_set_body(AST* func, AST* body)
{
    func->value.func.body = (AST*)gc_addref(body);
}

AST* FnApp(AST* fnapp)
{
    AST* node = ast(AST_FNAPP);
    node->value.fnapp.fn = (AST*)gc_addref(fnapp);
    vec_init(&(node->value.fnapp.args));
    return node;
}

void fnapp_set_fn(AST* fnapp, AST* fn)
{
    AST* old = fnapp->value.fnapp.fn;
    fnapp->value.fnapp.fn = (AST*)gc_addref(fn);
    gc_delref(old);
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
    vec_push_back(&(fnapp->value.fnapp.args), gc_addref(arg));
}

AST* Let(AST* temp, AST* val, AST* body)
{
    AST* node = ast(AST_LET);
    node->value.let.temp  = (AST*)gc_addref(temp);
    node->value.let.value = (AST*)gc_addref(val);
    node->value.let.body  = (AST*)gc_addref(body);
    return node;
}

AST* let_var(AST* let)
{
    return let->value.let.temp;
}

AST* let_val(AST* let)
{
    return let->value.let.value;
}

AST* let_body(AST* let)
{
    return let->value.let.body;
}

void let_set_body(AST* let, AST* body)
{
    let->value.let.body = (AST*)gc_addref(body);
}

AST* TempVar(void)
{
    static intptr_t val = 0;
    AST* node = ast(AST_TEMP);
    node->value.integer = val++;
    return node;
}
