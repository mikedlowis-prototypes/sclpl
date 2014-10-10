/**
  @file prettyprint.c
  @brief See header for details
  $Revision$
  $HeadURL$
  */
#include "pprint.h"

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

void pprint_token_type(FILE* file, lex_tok_t* token) {
    fprintf(file, "%s", token_type_to_string(token->type));
}

void pprint_token_value(FILE* file, lex_tok_t* token) {
    void* value = token->value;
    switch(token->type) {
        case T_STRING: fprintf(file, "'%s'", ((char*)value));      break;
        case T_CHAR:   fprintf(file, "\\%c", ((char)value));       break;
        case T_INT:    fprintf(file, "%d",   *((long int*)value)); break;
        case T_FLOAT:  fprintf(file, "%f",   *((double*)value));   break;
        case T_BOOL:   fprintf(file, "%b",   ((bool)value));       break;
        case T_VAR:    fprintf(file, "%s",   ((char*)value));      break;
        default:       fprintf(file, "???");                       break;
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


void pprint_tree(FILE* file, tree_t* tree)
{

}

