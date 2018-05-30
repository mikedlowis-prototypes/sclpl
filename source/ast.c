#include <sclpl.h>

static AST* ast(ASTType type) {
    AST* tree = emalloc(sizeof(AST));
    memset(tree, 0, sizeof(AST));
    tree->nodetype = type;
    return tree;
}

AST* String(Tok* val) {
    AST* node = ast(AST_STRING);
    node->value.text = val->value.text;
    return node;
}

char* string_value(AST* val) {
    assert(val->nodetype == AST_STRING);
    return val->value.text;
}

AST* Symbol(Tok* val) {
    AST* node = ast(AST_SYMBOL);
    node->value.text = val->value.text;
    return node;
}

char* symbol_value(AST* val) {
    assert(val->nodetype == AST_SYMBOL);
    return val->value.text;
}

AST* Char(Tok* val) {
    AST* node = ast(AST_CHAR);
    node->value.character = val->value.character;
    return node;
}

uint32_t char_value(AST* val) {
    assert(val->nodetype == AST_CHAR);
    return val->value.character;
}

AST* Integer(Tok* val) {
    AST* node = ast(AST_INT);
    node->value.integer = val->value.integer;
    return node;
}

intptr_t integer_value(AST* val) {
    assert(val->nodetype == AST_INT);
    return val->value.integer;
}

AST* Float(Tok* val) {
    AST* node = ast(AST_FLOAT);
    node->value.floating = val->value.floating;
    return node;
}

double float_value(AST* val) {
    assert(val->nodetype == AST_FLOAT);
    return val->value.floating;
}

AST* Bool(Tok* val) {
    AST* node = ast(AST_BOOL);
    node->value.boolean = val->value.boolean;
    return node;
}

bool bool_value(AST* val) {
    assert(val->nodetype == AST_BOOL);
    return val->value.boolean;
}

AST* Ident(Tok* val) {
    AST* node = ast(AST_IDENT);
    node->value.text = val->value.text;
    return node;
}

char* ident_value(AST* val) {
    assert(val->nodetype == AST_IDENT);
    return val->value.text;
}

AST* Var(Tok* name, AST* value, bool constant) {
    AST* node = ast(AST_VAR);
    node->value.var.name = name->value.text;
    node->value.var.value = value;
    node->value.var.constant = constant;
    return node;
}

char* var_name(AST* var) {
    assert(var->nodetype == AST_VAR);
    return var->value.var.name;
}

AST* var_value(AST* var) {
    assert(var->nodetype == AST_VAR);
    return var->value.var.value;
}

bool var_const(AST* var) {
    assert(var->nodetype == AST_VAR);
    return var->value.var.constant;
}
