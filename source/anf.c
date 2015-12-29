#include <sclpl.h>

static bool isatomic(AST* tree)
{
    switch (tree->type) {
        case AST_STRING:
        case AST_SYMBOL:
        case AST_IDENT:
        case AST_CHAR:
        case AST_INT:
        case AST_FLOAT:
        case AST_BOOL:
        case AST_FUNC:
            return true;
        default:
            return false;
    }
}

static bool isconst(AST* tree) {
    bool ret = isatomic(tree);
    if (!ret && tree->type == AST_FNAPP) {
        ret = isatomic(fnapp_fn(tree));
        for (int i = 0; i < vec_size(fnapp_args(tree)); i++) {
            ret = ret && isatomic(vec_at(fnapp_args(tree), i));
        }
    }
    return ret;
}

static AST* normalize_def(AST* tree)
{
    Tok name = { .value.text = def_name(tree) };
    return Def(&name, normalize(def_value(tree)));
}

static AST* normalize_fnapp(AST* tree)
{
    AST* normalized = tree;
    AST* fn = fnapp_fn(tree);
    /* Normalize the function */
    if (!isatomic(fn)) {
        AST* temp = TempVar();
        fnapp_set_fn(tree, temp);
        normalized = Let(temp, fn, tree);
    }
    /* Normalize the function arguments */
    vec_t temps;
    vec_init(&temps);
    vec_t* args = fnapp_args(tree);
    for (int i = 0; i < vec_size(args); i++) {
        AST* arg = (AST*)vec_at(args, i);
        if (!isatomic(arg)) {
            AST* temp = TempVar();
            vec_push_back(&temps, Let(temp, arg, NULL));
            vec_set(args, i, temp);
        }
    }
    /* Nest all the scopes and return the new form */
    for (int i = vec_size(&temps); i > 0; i--) {
        AST* let = (AST*)vec_at(&temps,i-1);
        let_set_body(let, normalized);
        normalized = let;
    }
    vec_deinit(&temps);
    return normalized;
}

static AST* normalize_if(AST* tree)
{
    AST* cond   = normalize(ifexpr_cond(tree));
    AST* thenbr = normalize(ifexpr_then(tree));
    AST* elsebr = normalize(ifexpr_else(tree));
    if (!isatomic(cond)) {
        AST* temp = TempVar();
        AST* body = IfExpr(); //(temp, thenbr, elsebr);
        ifexpr_set_cond(body, temp);
        ifexpr_set_then(body, thenbr);
        ifexpr_set_else(body, elsebr);
        tree = Let(temp, cond, body);
    } else {
        tree = IfExpr(); //(cond, thenbr, elsebr);
        ifexpr_set_cond(tree, cond);
        ifexpr_set_then(tree, thenbr);
        ifexpr_set_else(tree, elsebr);
    }
    return tree;
}

static AST* normalize_func(AST* tree)
{
    func_set_body(tree, normalize(func_body(tree)));
    return tree;
}

static AST* normalize_let(AST* tree)
{
    AST* var  = let_var(tree);
    AST* val  = normalize(let_val(tree));
    AST* body = normalize(let_body(tree));
    /* Find the inner most let block */
    if (!isconst(val)) {
        AST* let = val;
        while (let->type == AST_LET && let_body(let)->type == AST_LET)
            let = let_body(let);
        let_set_body(let, Let(var, let_body(let), body));
        tree = let;
    } else {
        tree = Let(var, val, body);
    }
    return tree;
}

AST* normalize(AST* tree)
{
    if (NULL == tree)
        return tree;
    switch (tree->type)
    {
        case AST_DEF:   tree = normalize_def(tree);   break;
        case AST_FNAPP: tree = normalize_fnapp(tree); break;
        case AST_IF:    tree = normalize_if(tree);    break;
        case AST_FUNC:  tree = normalize_func(tree);  break;
        case AST_LET:   tree = normalize_let(tree);   break;
        default: break;
    }
    return tree;
}

