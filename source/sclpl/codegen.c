#include "parser.h"
#include "codegen.h"
#include "pprint.h"

static tree_t* get_child(tree_t* p_tree, size_t idx) {
    tree_t* child = NULL;
    if (p_tree->tag == TREE) {
        vec_t* vec = p_tree->ptr.vec;
        if (idx < vec_size(vec))
            child = vec_at(vec, idx);
    }
    return child;
}

static void* get_val(tree_t* p_tree) {
    void* ret = NULL;
    if (p_tree->tag == ATOM) {
        ret = p_tree->ptr.tok->value;
    }
    return ret;
}

static void* get_child_val(tree_t* p_tree, size_t idx) {
    void* ret = NULL;
    tree_t* child = get_child(p_tree,idx);
    if (child != NULL) {
        ret = get_val(child);
    }
    return ret;
}

static bool is_formtype(tree_t* p_tree, const char* val) {
    bool ret = false;
    tree_t* child = get_child(p_tree, 0);
    if ((NULL != child) && (child->tag == ATOM)) {
        lex_tok_t* token = child->ptr.tok;
        if ((token->type == T_VAR) &&
            (0 == strcmp(val, (char*)token->value))) {
            ret = true;
        }
    }
    return ret;
}

/*****************************************************************************/

static void lift_funcs(vec_t* fnlst, tree_t* tree) {
    if (is_formtype(tree, "fn"))
        vec_push_back(fnlst, mem_retain(tree));

    if (tree->tag == TREE) {
        vec_t* p_vec = tree->ptr.vec;
        for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
            lift_funcs(fnlst, (tree_t*)vec_at(p_vec, idx));
        }
    }
}

static vec_t* find_fn_literals(vec_t* prgrm) {
    vec_t* fnlst = vec_new(0);
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* tree = (tree_t*)vec_at(prgrm, idx);
        if (!is_formtype(tree, "require")) {
            lift_funcs(fnlst, tree);
        }
    }
    return fnlst;
}

static size_t get_fn_id(vec_t* funcs, tree_t* fn) {
    size_t idx;
    for (idx = 0; idx < vec_size(funcs); idx++) {
        if (fn == vec_at(funcs,idx)) {
            break;
        }
    }
    return idx;
}

/*****************************************************************************/

static void print_indent(int depth) {
    for(int i = 0; i < (4 * depth); i++)
        printf("%c", ' ');
}

static void print_char(char ch) {
    switch (ch) {
        case '\r': printf("__char('\\r')");    break;
        case '\n': printf("__char('\\n')");    break;
        case '\t': printf("__char('\\t')");    break;
        case '\v': printf("__char('\\v')");    break;
        default:   printf("__char('%c')", ch); break;
    }
}

static void print_string(const char* str) {
    printf("__string(\"");
    while('\0' != str[0])
    {
        switch (str[0]) {
            case '\r': printf("\\r");        break;
            case '\n': printf("\\n");        break;
            case '\t': printf("\\t");        break;
            case '\v': printf("\\v");        break;
            default:   printf("%c", str[0]); break;
        }
        str++;
    }
    printf("\")");
}

/*****************************************************************************/

static void emit_header(void) {
    puts("#include <sclpl.h>\n");
}

static void emit_fn_signature(char* name, tree_t* fnval) {
    printf("_Value %s(", name);
    vec_t* params = get_child(fnval, 1)->ptr.vec;
    for (size_t i = 0; i < vec_size(params); i++) {
        printf("_Value %s", (char*)get_val((tree_t*)vec_at(params,i)));
        if (i+1 < vec_size(params))
            printf(", ");
    }
    printf(")");
}

static void emit_def_placeholders(vec_t* prgrm) {
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "def")) {
            printf("_Value %s;\n", (char*)get_child_val(p_tree,1));
        }
    }
    puts("");
}

static void emit_expression(vec_t* fnlst, tree_t* p_tree, int depth) {
    if (p_tree->tag == ATOM) {
        lex_tok_t* tok = p_tree->ptr.tok;
        switch (tok->type) {
            case T_STRING: print_string(((char*)tok->value));                        break;
            case T_CHAR:   print_char(((char)(int)tok->value));                      break;
            case T_INT:    printf("__int(%ld)",   *((long int*)tok->value));         break;
            case T_FLOAT:  printf("__float(%f)",  *((double*)tok->value));           break;
            case T_BOOL:   printf("__bool(%s)",   ((int)tok->value)?"true":"false"); break;
            case T_VAR:    printf("%s",           ((char*)tok->value));              break;
        }
    } else if (is_formtype(p_tree, "if")) {
        printf("IF (");
        emit_expression(fnlst, get_child(p_tree, 1), depth);
        printf(")\n");
        print_indent(depth+1);
        emit_expression(fnlst, get_child(p_tree, 2), depth+1);
        printf("\n");
        print_indent(depth);
        printf("ELSE\n");
        print_indent(depth+1);
        if (vec_size(p_tree->ptr.vec) > 3) {
            emit_expression(fnlst, get_child(p_tree, 4), depth+1);
        } else {
            printf("NIL");
        }

    } else if (is_formtype(p_tree, "fn")) {
        printf("__func(&fn%d)", get_fn_id(fnlst, p_tree));
    } else {
        vec_t* vec   = p_tree->ptr.vec;
        size_t nargs = vec_size(vec)-1;
        /* Determine the calling convention based on number of args */
        if (0 == nargs)
            printf("__call0(%s", (char*)get_val(vec_at(vec,0)));
        else if (nargs < 16)
            printf("__calln(%s, %d, ", (char*)get_val(vec_at(vec,0)), nargs);
        else
            printf("__calln(%s, n, ", (char*)get_val(vec_at(vec,0)));
        /* Print out the arguments */
        for (size_t idx = 1; idx < vec_size(vec); idx++) {
            emit_expression(fnlst, (tree_t*)vec_at(vec,idx), depth);
            if (idx+1 < vec_size(vec))
                printf(", ");
        }
        printf(")");
    }
}

static void emit_fn_declarations(vec_t* fnlst) {
    char name[64];
    for (size_t idx = 0; idx < vec_size(fnlst); idx++) {
        sprintf(name,"fn%d", idx);
        printf("static ");
        emit_fn_signature(name, (tree_t*)vec_at(fnlst,idx));
        puts(";");
    }
    puts("");
}

static void emit_fn_definitions(vec_t* fnlst) {
    char name[64];
    for (size_t idx = 0; idx < vec_size(fnlst); idx++) {
        tree_t* func = (tree_t*)vec_at(fnlst,idx);
        sprintf(name,"fn%d", idx);
        printf("static ");
        emit_fn_signature(name, func);
        puts(" {");

        vec_t* body = (vec_t*)func->ptr.vec;
        for (size_t i = 2; i < vec_size(body); i++) {
            printf("    ");
            if (i+1 == vec_size(body))
                printf("return ");
            emit_expression(fnlst, (tree_t*)vec_at(body,i), 1);
            printf(";\n");
        }
        puts("}\n");
    }
}

static void emit_toplevel(vec_t* fnlst, vec_t* prgrm) {
    puts("void toplevel(void) {");
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "require")) {
            printf("    extern void %s_toplevel(void);\n", (char*)get_child_val(p_tree,1));
            printf("    %s_toplevel();\n", (char*)get_child_val(p_tree,1));
        } else if (is_formtype(p_tree, "def")) {
            printf("    %s = ", (char*)get_child_val(p_tree,1));
            emit_expression(fnlst, get_child(p_tree, 2), 0);
            printf(";\n");
        } else {
            printf("    (void)(");
            emit_expression(fnlst, p_tree, 1);
            printf(");\n");
        }
    }
    puts("}");
}

static void emit_footer(void) {
    puts(
        "\nint main(int argc, char** argv) {"
        "\n    (void)argc;"
        "\n    (void)argv;"
        "\n    toplevel();"
        "\n    return 0;"
        "\n}"
    );
}

void codegen_csource(FILE* file, vec_t* program) {
    emit_header();
    emit_def_placeholders(program);
    vec_t* funcs = find_fn_literals(program);
    emit_fn_declarations(funcs);
    emit_fn_definitions(funcs);
    emit_toplevel(funcs, program);
    mem_release(funcs);
    emit_footer();
}
