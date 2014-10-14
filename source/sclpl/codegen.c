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

static void print_indent(int depth) {
    for(int i = 0; i < (4 * depth); i++)
        printf("%c", ' ');
}

static void emit_header(void) {
    puts("#include <sclpl.h>\n");
}

static void emit_fn_signature(char* name, tree_t* fnval) {
    printf("val %s(", name);
    vec_t* params = get_child(fnval, 1)->ptr.vec;
    for (size_t i = 0; i < vec_size(params); i++) {
        printf("val %s", (char*)get_val((tree_t*)vec_at(params,i)));
        if (i+1 < vec_size(params))
            printf(", ");
    }
    printf(")");
}

static void emit_def_placeholders(vec_t* prgrm) {
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "def")) {
            printf("val %s;\n", (char*)get_child_val(p_tree,1));
        }
    }
    puts("");
}

static void emit_expression(tree_t* p_tree, int depth) {
    if (p_tree->tag == ATOM) {
        lex_tok_t* tok = p_tree->ptr.tok;
        switch (tok->type) {
            case T_STRING: printf("'%s'", ((char*)tok->value));              break;
            case T_CHAR:   printf("\\%c", ((char)(int)tok->value));          break;
            case T_INT:    printf("%ld",  *((long int*)tok->value));         break;
            case T_FLOAT:  printf("%f",   *((double*)tok->value));           break;
            case T_BOOL:   printf("%s",   ((int)tok->value)?"true":"false"); break;
            case T_VAR:    printf("%s",   ((char*)tok->value));              break;
        }
    } else if (is_formtype(p_tree, "if")) {
        printf("IF(");
        emit_expression(get_child(p_tree, 1), depth);
        printf(")\n");
        print_indent(depth+1);
        emit_expression(get_child(p_tree, 2), depth+1);

        if (vec_size(p_tree->ptr.vec) > 3) {
            printf("\n");
            print_indent(depth);
            printf("ELSE\n");
            print_indent(depth+1);
            emit_expression(get_child(p_tree, 4), depth+1);
        }

    } else if (is_formtype(p_tree, "fn")) {
        printf("<func>");
    } else {
        vec_t* vec = p_tree->ptr.vec;
        printf("%s(", (char*)get_val(vec_at(vec,0)));
        for (size_t idx = 1; idx < vec_size(vec); idx++) {
            emit_expression((tree_t*)vec_at(vec,idx), depth);
            if (idx+1 < vec_size(vec))
                printf(", ");
        }
        printf(")");
    }
}

static void emit_toplevel(vec_t* prgrm) {
    puts("void toplevel(void) {");
    for (size_t idx = 0; idx < vec_size(prgrm); idx++) {
        tree_t* p_tree = (tree_t*)vec_at(prgrm, idx);
        if (is_formtype(p_tree, "require")) {
            printf("    extern void %s_toplevel(void);\n", (char*)get_child_val(p_tree,1));
            printf("    %s_toplevel();\n", (char*)get_child_val(p_tree,1));
        } else if (is_formtype(p_tree, "def")) {
            printf("    %s = ", (char*)get_child_val(p_tree,1));
            emit_expression(get_child(p_tree, 2), 0);
            printf(";\n");
        } else {
            printf("    ");
            emit_expression(p_tree, 1);
            printf(";\n");
        }
    }
    puts("}");
}

static void emit_footer(void) {

}

void codegen_csource(FILE* file, vec_t* program) {
    emit_header();
    emit_def_placeholders(program);
    emit_toplevel(program);
    emit_footer();
}
