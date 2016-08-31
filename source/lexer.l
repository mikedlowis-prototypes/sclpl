%{
#include <sclpl.h>

static union {
    char* text;
    uint32_t character;
    intptr_t integer;
    double floating;
    bool boolean;
} Value;

static char* dupstring(const char* old) {
    size_t length = strlen(old);
    char* str = (char*)gc_alloc(length+1, NULL);
    memcpy(str, old, length);
    str[length] = '\0';
    return str;
}

%}

DIGIT   [0-9]
ALPHA   [a-zA_Z]
ALPHA_  [a-zA_Z_]
ALNUM_  [a-zA-Z0-9_]
SPACE   [ \t\r\n]
NOSPACE [^ \t\r\n]

%option noyywrap

%%

<<EOF>> { return T_END_FILE; }

"end" { return T_END;    }
"("   { return T_LPAR;   }
")"   { return T_RPAR;   }
"["   { return T_LBRACK; }
"]"   { return T_RBRACK; }
"{"   { return T_LBRACE; }
"}"   { return T_RBRACE; }
";"   { return T_END;    }
","   { return T_COMMA;  }
"'"   { return T_SQUOTE; }
":"   { return T_COLON;  }
"&"   { return T_AMP;    }

\\.       { Value.character = yytext[1];    return T_CHAR; }
\\space   { Value.character = ' ';          return T_CHAR; }
\\newline { Value.character = '\n';         return T_CHAR; }
\\return  { Value.character = '\r';         return T_CHAR; }
\\tab     { Value.character = '\t';         return T_CHAR; }
\\vtab    { Value.character = '\v';         return T_CHAR; }
\\[a-z]+  { Value.text = dupstring(yytext); return T_ID;   }

0b[01]+ {
    Value.integer = strtol(&yytext[2], NULL, 2);
    return T_INT;
}

0o[0-7]+ {
    Value.integer = strtol(&yytext[2], NULL, 8);
    return T_INT;
}

0d[0-9]+ {
    Value.integer = strtol(&yytext[2], NULL, 10);
    return T_INT;
}

0h[0-9a-fA-F]+ {
    Value.integer = strtol(&yytext[2], NULL, 16);
    return T_INT;
}

0[b0dh][0-9a-fA-F]+ {
    Value.text = dupstring(yytext);
    return T_ID;
}

[+-]?[0-9]+ {
    Value.integer = strtol(&yytext[0], NULL, 10);
    return T_INT;
}

[+-]?[0-9]+\.[0-9]+(e[+-]?[0-9]+)? {
    Value.floating = strtod(yytext, NULL);
    return T_FLOAT;
}

\"([^"]|\\\")*\" {
    size_t len = strlen(&yytext[1]);
    Value.text = dupstring(&yytext[1]);
    Value.text[len-1] = '\0';
    return T_STRING;
}

true  {
    Value.boolean = true;
    return T_BOOL;
}

false {
    Value.boolean = false;
    return T_BOOL;
}

"require" { return T_REQUIRE; }
"def"     { return T_DEF;     }
"if"      { return T_IF;      }
"fn"      { return T_FN;      }
"then"    { return T_THEN;    }
"else"    { return T_ELSE;    }

[a-zA-Z_][a-zA-Z0-9_]* {
    Value.text = dupstring(yytext);
    return T_ID;
}

[^ \r\t\n\[\]\{\}\(\)'\",;:]+ {
    Value.text = dupstring(yytext);
    return T_ID;
}

%%

static void token_free(void* obj)
{
    Tok* tok = (Tok*)obj;
    if ((tok->type != T_BOOL) &&
        (tok->type != T_CHAR) &&
        (tok->type != T_INT) &&
        (tok->type != T_FLOAT) &&
        (NULL != tok->value.text))
        gc_delref(tok->value.text);
}

Tok* gettoken(Parser* ctx)
{
    Tok* tok = NULL;
    int type = yylex();
    if (type != T_END_FILE) {
        tok = (Tok*)gc_alloc(sizeof(Tok), &token_free);
        tok->type = type;
        memcpy(&(tok->value), &Value, sizeof(Value));
    }
    return tok;
}

void fetchline(Parser* ctx)
{
}

