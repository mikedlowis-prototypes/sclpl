#include <sclpl.h>

static AST* ast(ASTType type) {
    AST* tree = emalloc(sizeof(AST));
    memset(tree, 0, sizeof(AST));
    tree->type = type;
    return tree;
}

AST* String(Tok* val) {
    AST* node = ast(AST_STRING);
    node->value.text = val->value.text;
    return node;
}

char* string_value(AST* val) {
    assert(val->type == AST_STRING);
    return val->value.text;
}

AST* Symbol(Tok* val) {
    AST* node = ast(AST_SYMBOL);
    node->value.text = val->value.text;
    return node;
}

char* symbol_value(AST* val) {
    assert(val->type == AST_SYMBOL);
    return val->value.text;
}

AST* Char(Tok* val) {
    AST* node = ast(AST_CHAR);
    node->value.character = val->value.character;
    return node;
}

uint32_t char_value(AST* val) {
    assert(val->type == AST_CHAR);
    return val->value.character;
}

AST* Integer(Tok* val) {
    AST* node = ast(AST_INT);
    node->value.integer = val->value.integer;
    return node;
}

intptr_t integer_value(AST* val) {
    assert(val->type == AST_INT);
    return val->value.integer;
}

AST* Float(Tok* val) {
    AST* node = ast(AST_FLOAT);
    node->value.floating = val->value.floating;
    return node;
}

double float_value(AST* val) {
    assert(val->type == AST_FLOAT);
    return val->value.floating;
}

AST* Bool(Tok* val) {
    AST* node = ast(AST_BOOL);
    node->value.boolean = val->value.boolean;
    return node;
}

bool bool_value(AST* val) {
    assert(val->type == AST_BOOL);
    return val->value.boolean;
}

AST* Ident(Tok* val) {
    AST* node = ast(AST_IDENT);
    node->value.text = val->value.text;
    return node;
}

char* ident_value(AST* val) {
    assert(val->type == AST_IDENT);
    return val->value.text;
}

AST* Let(Tok* name, AST* value) {
    AST* node = ast(AST_LET);
    node->value.let.name = name->value.text;
    node->value.let.value = value;
    return node;
}

char* let_name(AST* let) {
    assert(let->type == AST_LET);
    return let->value.let.name;
}

AST* let_value(AST* let) {
    assert(let->type == AST_LET);
    return let->value.let.value;
}
