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

static void print_indent(FILE* file, int depth) {
    for(int i = 0; i < (4 * depth); i++)
        fprintf(file, "%c", ' ');
}

static void print_char(FILE* file, char ch) {
    switch (ch) {
        case '\r': fprintf(file, "__char('\\r')");    break;
        case '\n': fprintf(file, "__char('\\n')");    break;
        case '\t': fprintf(file, "__char('\\t')");    break;
        case '\v': fprintf(file, "__char('\\v')");    break;
        default:   fprintf(file, "__char('%c')", ch); break;
    }
}

static void print_string(FILE* file, const char* str) {
    fprintf(file, "__string(\"");
    while('\0' != str[0])
    {
        switch (str[0]) {
            case '\r': fprintf(file, "\\r");        break;
            case '\n': fprintf(file, "\\n");        break;
            case '\t': fprintf(file, "\\t");        break;
            case '\v': fprintf(file, "\\v");        break;
            default:   fprintf(file, "%c", str[0]); break;
        }
        str++;
    }
    fprintf(file, "\")");
}

/*****************************************************************************/

static void emit_header(FILE* file) {
    fputs("#include \"sclpl.h\"\n\n", file);
}

static void emit_fn_signature(FILE* file, char* name, tree_t* fnval) {
    fprintf(file, "_Value %s(", name);
    vec_t* params = get_child(fnval, 1)->ptr.vec;
    for (size_t i = 0; i < vec_size(params); i++) {
        fprintf(file, "_Value %s", (char*)get_val((tree_t*)vec_at(params,i)));
        if (i+1 < vec_size(params))
            fprintf(file, ", ");
    }
    fprintf(file, ")");
}

static void emit_def_placeholders(FILE* file, vec_t* prgrm) {
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "def")) {
            fprintf(file, "_Value %s;\n", (char*)get_child_val(p_tree,1));
        }
    }
    fputs("\n", file);
}

static void emit_expression(FILE* file, vec_t* fnlst, tree_t* p_tree, int depth) {
    if (p_tree->tag == ATOM) {
        lex_tok_t* tok = p_tree->ptr.tok;
        switch (tok->type) {
            case T_STRING: print_string(file, ((char*)tok->value));                             break;
            case T_CHAR:   print_char(file, ((char)(intptr_t)tok->value));                      break;
            case T_INT:    fprintf(file, "__int(%ld)",  *((long int*)tok->value));              break;
            case T_FLOAT:  fprintf(file, "__float(%f)", *((double*)tok->value));                break;
            case T_BOOL:   fprintf(file, "__bool(%s)",  ((intptr_t)tok->value)?"true":"false"); break;
            case T_VAR:    fprintf(file, "%s",          ((char*)tok->value));                   break;
            default:                                                                            break;
        }
    } else if (is_formtype(p_tree, "if")) {
        fprintf(file, "IF (");
        emit_expression(file, fnlst, get_child(p_tree, 1), depth);
        fprintf(file, ")\n");
        print_indent(file, depth+1);
        emit_expression(file, fnlst, get_child(p_tree, 2), depth+1);
        fprintf(file, "\n");
        print_indent(file, depth);
        fprintf(file, "ELSE\n");
        print_indent(file, depth+1);
        if (vec_size(p_tree->ptr.vec) > 3) {
            emit_expression(file, fnlst, get_child(p_tree, 4), depth+1);
        } else {
            fprintf(file, "__nil");
        }

    } else if (is_formtype(p_tree, "fn")) {
        fprintf(file, "__func(&fn%d)", (int)get_fn_id(fnlst, p_tree));
    } else {
        vec_t* vec   = p_tree->ptr.vec;
        int nargs = vec_size(vec)-1;
        /* Determine the calling convention based on number of args */
        if (0 == nargs)
            fprintf(file, "__call0(%s", (char*)get_val(vec_at(vec,0)));
        else if (nargs < 16)
            fprintf(file, "__calln(%s, %d, ", (char*)get_val(vec_at(vec,0)), (int)nargs);
        else
            fprintf(file, "__calln(%s, n, ", (char*)get_val(vec_at(vec,0)));
        /* Print out the arguments */
        for (size_t idx = 1; idx < vec_size(vec); idx++) {
            emit_expression(file, fnlst, (tree_t*)vec_at(vec,idx), depth);
            if (idx+1 < vec_size(vec))
                fprintf(file, ", ");
        }
        fprintf(file, ")");
    }
}

static void emit_fn_declarations(FILE* file, vec_t* fnlst) {
    char name[64];
    for (size_t idx = 0; idx < vec_size(fnlst); idx++) {
        sprintf(name,"fn%d", (int)idx);
        fprintf(file, "static ");
        emit_fn_signature(file, name, (tree_t*)vec_at(fnlst,idx));
        fputs(";\n", file);
    }
    fputs("\n", file);
}

static void emit_fn_definitions(FILE* file, vec_t* fnlst) {
    char name[64];
    for (size_t idx = 0; idx < vec_size(fnlst); idx++) {
        tree_t* func = (tree_t*)vec_at(fnlst,idx);
        sprintf(name,"fn%d", (int)idx);
        fprintf(file, "static ");
        emit_fn_signature(file, name, func);
        fputs(" {\n", file);

        vec_t* body = (vec_t*)func->ptr.vec;
        for (size_t i = 2; i < vec_size(body); i++) {
            fprintf(file, "    ");
            if (i+1 == vec_size(body))
                fprintf(file, "return ");
            emit_expression(file, fnlst, (tree_t*)vec_at(body,i), 1);
            fprintf(file, ";\n");
        }
        fputs("}\n\n", file);
    }
}

static void emit_toplevel(FILE* file, vec_t* fnlst, vec_t* prgrm) {
    fputs("void toplevel(void) {\n", file);
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "require")) {
            fprintf(file, "    extern void %s_toplevel(void);\n", (char*)get_child_val(p_tree,1));
            fprintf(file, "    %s_toplevel();\n", (char*)get_child_val(p_tree,1));
        } else if (is_formtype(p_tree, "def")) {
            fprintf(file, "    %s = ", (char*)get_child_val(p_tree,1));
            emit_expression(file, fnlst, get_child(p_tree, 2), 0);
            fprintf(file, ";\n");
        } else {
            fprintf(file, "    (void)(");
            emit_expression(file, fnlst, p_tree, 1);
            fprintf(file, ");\n");
        }
    }
    fputs("}\n", file);
}

static void emit_footer(FILE* file) {
    fputs(
        "\nint main(int argc, char** argv) {"
        "\n    (void)argc;"
        "\n    (void)argv;"
        "\n    toplevel();"
        "\n    return 0;"
        "\n}\n",
        file
    );
}

void codegen_csource(FILE* file, vec_t* program) {
    (void)file;
    emit_header(file);
    emit_def_placeholders(file, program);
    vec_t* funcs = find_fn_literals(program);
    emit_fn_declarations(file, funcs);
    emit_fn_definitions(file, funcs);
    emit_toplevel(file, funcs, program);
    mem_release(funcs);
    emit_footer(file);
}
