#include <sclpl.h>

void codegen(FILE* file, AST* tree)
{
    switch(tree->type) {
        case AST_STRING:
            fprintf(file, "\"%s\"", string_value(tree));
            break;

        case AST_SYMBOL:
            //fprintf(file, "__symbol(\"%s\")", symbol_value(tree));
            break;

        case AST_CHAR:
            //fprintf(file, "'%s'", char_value(tree));
            break;

        case AST_INT:
            fprintf(file, "%ld", integer_value(tree));
            break;

        case AST_FLOAT:
            fprintf(file, "%f", float_value(tree));
            break;

        case AST_BOOL:
            fprintf(file, "%s", bool_value(tree) ? "true" : "false");
            break;

        case AST_IDENT:
            fprintf(file, "%s", ident_value(tree));
            break;

        case AST_TEMP:
            fprintf(file, "_t%lu", temp_value(tree));
            break;

        case AST_DEF:
            if (def_value(tree)->type == AST_FUNC) {
                fprintf(file, "val %s", def_name(tree));
                codegen(file, def_value(tree));
            } else {
                fprintf(file, "val %s", def_name(tree));
                fprintf(file, " = ");
                codegen(file, def_value(tree));
                fprintf(file, ";");
            }
            break;

        case AST_IF:
            fprintf(file,"    if (");
            codegen(file, ifexpr_cond(tree));
            fprintf(file,")\n");
            codegen(file, ifexpr_then(tree));
            if (ifexpr_else(tree)) {
                fprintf(file,"\n    else\n");
                codegen(file, ifexpr_else(tree));
            } else {
                fprintf(file,"    {return nil;}");
            }
            break;

        case AST_FUNC:
            fprintf(file,"(");
            for (size_t i = 0; i < vec_size(func_args(tree)); i++) {
                fprintf(file,"val ");
                codegen(file, vec_at(func_args(tree), i));
                if (i+1 < vec_size(func_args(tree)))
                    fprintf(file,", ");
            }
            fprintf(file,") {\n");
            codegen(file, func_body(tree));
            fprintf(file,"\n}\n");
            break;

        case AST_FNAPP:
            codegen(file, fnapp_fn(tree));
            fprintf(file,"(");
            for (size_t i = 0; i < vec_size(fnapp_args(tree)); i++) {
                codegen(file, vec_at(fnapp_args(tree), i));
                if (i+1 < vec_size(fnapp_args(tree)))
                    fprintf(file,",");
            }
            fprintf(file,")");
            break;

        case AST_LET:
            fprintf(file,"    {val ");
            codegen(file, let_var(tree));
            fprintf(file," = ");
            codegen(file, let_val(tree));
            fprintf(file,";\n");
            if (let_body(tree)->type != AST_LET && let_body(tree)->type != AST_IF) {
                fprintf(file,"    return ");
                codegen(file, let_body(tree));
                fprintf(file,";");
            } else {
                codegen(file, let_body(tree));
            }
            fprintf(file,"}");
            break;

        case AST_REQ:
        default:
            break;
    }
}
