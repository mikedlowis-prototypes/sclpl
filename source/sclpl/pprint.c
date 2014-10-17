/**
  @file prettyprint.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "pprint.h"

static void print_indent(FILE* file, int depth) {
    for(int i = 0; i < (2 * depth); i++)
        fprintf(file, "%c", ' ');
}

static const char* token_type_to_string(lex_tok_type_t type) {
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
        case T_VAR:      return "T_VAR";
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

void pprint_token_type(FILE* file, lex_tok_t* token) {
    fprintf(file, "%s", token_type_to_string(token->type));
}

void pprint_token_value(FILE* file, lex_tok_t* token) {
    void* value = token->value;
    switch(token->type) {
        case T_STRING: fprintf(file, "%s", ((char*)value));              break;
        case T_CHAR:   print_char(file, ((char)(int)value));               break;
        case T_INT:    fprintf(file, "%ld",  *((long int*)value));         break;
        case T_FLOAT:  fprintf(file, "%f",   *((double*)value));           break;
        case T_BOOL:   fprintf(file, "%s",   ((int)value)?"true":"false"); break;
        case T_VAR:    fprintf(file, "%s",   ((char*)value));              break;
        default:       fprintf(file, "???");                               break;
    }
}

void pprint_token(FILE* file, lex_tok_t* token)
{
    pprint_token_type(file, token);
    if (token->type < T_LBRACE) {
        fprintf(file, ":");
        pprint_token_value(file, token);
    }
    fprintf(file, "\n");
}


void pprint_tree(FILE* file, tree_t* tree, int depth)
{
    print_indent(file, depth);
    if (tree->tag == ATOM) {
        pprint_token(file, tree->ptr.tok);
    } else {
        puts("(tree");
        vec_t* p_vec = tree->ptr.vec;
        for(size_t idx = 0; idx < vec_size(p_vec); idx++) {
            pprint_tree(file, (tree_t*)vec_at(p_vec, idx), depth+1);
        }
        print_indent(file, depth);
        puts(")");
    }
}

