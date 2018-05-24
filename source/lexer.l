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
    char* str = emalloc(length+1);
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

"let"  { return T_LET;    }
"if"   { return T_IF;     }
"then" { return T_THEN;   }
"else" { return T_ELSE;   }
"end"  { return T_END;    }
"("    { return T_LPAR;   }
")"    { return T_RPAR;   }
"["    { return T_LBRACK; }
"]"    { return T_RBRACK; }
"{"    { return T_LBRACE; }
"}"    { return T_RBRACE; }
";"    { return T_END;    }
","    { return T_COMMA;  }
"'"    { return T_SQUOTE; }
":"    { return T_COLON;  }
"&"    { return T_AMP;    }
"="    { return T_ASSIGN; }

\\.       { Value.character = yytext[1];    return T_CHAR; }
\\space   { Value.character = ' ';          return T_CHAR; }
\\newline { Value.character = '\n';         return T_CHAR; }
\\return  { Value.character = '\r';         return T_CHAR; }
\\tab     { Value.character = '\t';         return T_CHAR; }
\\vtab    { Value.character = '\v';         return T_CHAR; }

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

[a-zA-Z_][a-zA-Z0-9_]* {
    Value.text = dupstring(yytext);
    return T_ID;
}

. { return T_ERROR; }

%%

void gettoken(Parser* ctx, Tok* tok) {
    tok->type = yylex();
    if (tok->type != T_END_FILE)
        memcpy(&(tok->value), &Value, sizeof(Value));
}
