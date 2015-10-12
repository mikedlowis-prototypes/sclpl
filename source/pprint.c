/**
  @file prettyprint.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include <sclpl.h>

static void print_indent(FILE* file, int depth) {
    for(int i = 0; i < (2 * depth); i++)
        fprintf(file, "%c", ' ');
}

static const char* token_type_to_string(TokType type) {
    switch(type) {
        case T_STRING:   return "T_STRING";
        case T_CHAR:     return "T_CHAR";
        case T_INT:      return "T_INT";
        case T_FLOAT:    return "T_FLOAT";
        case T_BOOL:     return "T_BOOL";
        case T_LBRACE:   return "T_LBRACE";
        case T_RBRACE:   return "T_RBRACE";
        case T_LBRACK:   return "T_LBRACK";
        case T_RBRACK:   return "T_RBRACK";
        case T_LPAR:     return "T_LPAR";
        case T_RPAR:     return "T_RPAR";
        case T_COMMA:    return "T_COMMA";
        case T_ID:       return "T_ID";
        case T_END:      return "T_END";
        case T_SQUOTE:   return "T_SQUOTE";
        case T_DQUOTE:   return "T_DQUOTE";
        case T_END_FILE: return "T_END_FILE";
        default:         return "???";
    }
}

static void print_char(FILE* file, char ch) {
    int i;
    static const char* lookup_table[5] = {
        " \0space",
        "\n\0newline",
        "\r\0return",
        "\t\0tab",
        "\v\0vtab"
    };
    for(i = 0; i < 5; i++) {
        if (ch == lookup_table[i][0]) {
            fprintf(file, "\\%s", &(lookup_table[i][2]));
            break;
        }
    }
    if (i == 5) fprintf(file, "\\%c", ch);
}

void pprint_token_type(FILE* file, Tok* token) {
    fprintf(file, "%s", token_type_to_string(token->type));
}

void pprint_token_value(FILE* file, Tok* token) {
    switch(token->type) {
        case T_STRING: fprintf(file, "\"%s\"", token->value.text);                 break;
        case T_ID:     fprintf(file, "%s", token->value.text);                     break;
        case T_CHAR:   print_char(file, token->value.character);                   break;
        case T_INT:    fprintf(file, "%ld", token->value.integer);                 break;
        case T_FLOAT:  fprintf(file, "%f", token->value.floating);                 break;
        case T_BOOL:   fprintf(file, "%s", (token->value.boolean)?"true":"false"); break;
        default:       fprintf(file, "???");                                       break;
    }
}

void pprint_token(FILE* file, Tok* token, bool print_loc)
{
    if (print_loc) {
        fprintf(file, "%zu:", token->line);
        fprintf(file, "%zu:", token->col);
    }
    pprint_token_type(file, token);
    if (token->type < T_LBRACE) {
        fprintf(file, ":");
        pprint_token_value(file, token);
    }
    fprintf(file, "\n");
}

/*****************************************************************************/

static const char* tree_type_to_string(ASTType type) {
    switch(type) {
        case AST_STRING: return "T_STRING";
        case AST_SYMBOL: return "T_SYMBOL";
        case AST_IDENT:  return "T_IDENT";
        case AST_CHAR:   return "T_CHAR";
        case AST_INT:    return "T_INT";
        case AST_FLOAT:  return "T_FLOAT";
        case AST_BOOL:   return "T_BOOL";
        default:         return "???";
    }
}

static void pprint_literal(FILE* file, AST* tree, int depth)
{
    printf("%s:", tree_type_to_string(tree->type));
    switch(tree->type) {
        case AST_STRING: printf("\"%s\"", string_value(tree)); break;
        case AST_SYMBOL: printf("%s", symbol_value(tree));     break;
        case AST_IDENT:  printf("%s", ident_value(tree));      break;
        case AST_CHAR:   printf("%c", char_value(tree));       break;
        case AST_INT:    printf("%ld", integer_value(tree));   break;
        case AST_FLOAT:  printf("%lf", float_value(tree));     break;
        case AST_BOOL:
            printf("%s", bool_value(tree) ? "true" : "false");
            break;
        default: printf("???");
    }
}

void pprint_tree(FILE* file, AST* tree, int depth)
{
    print_indent(file, depth);
    if (tree->type <= AST_IDENT) {
        pprint_literal(file, tree, depth);
    } else {
        //fputs("(tree", file);
        //vec_t* p_vec = tree->ptr.vec;
        //for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
        //    pprint_tree(file, (AST*)vec_at(p_vec, idx), depth+1);
        //}
        //print_indent(file, depth);
        //fputs(")\n", file);
    }
}

