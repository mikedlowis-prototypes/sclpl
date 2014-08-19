
replexpr : <ws> <expr> /[^\n]*\n/ ;

expr : '(' <ws> "quote" <ws> <expr> <ws> ')'
     | '(' <ws> "quasiquote" <ws> <expr> <ws> ')'
     | '(' <ws> "unquote" <ws> <expr> <ws> ')'
     | '(' <ws> "if" <ws> <expr> <ws> <expr> <ws> (<expr> <ws>)?')'
     | '(' <ws> "fn" <ws> '(' (<ws> <var> <ws>)* ')' (<ws> <expr> <ws>)+ ')'
     | '(' <ws> "def" <ws> <var> <ws> <expr> <ws> ')'
     | '(' <ws> <expr> (<ws> <expr> <ws>)* ')'
     | /['`,]/ <expr>
     | <atom>
     ;

atom : <num> | <ch> | <str> | <bool> | <var> ;

num : "0b" /[0-1]+/
    | "0o" /[0-7]+/
    | "0d" /[0-9]+/
    | "0x" /[0-9a-fA-F]+/
    | /[-+]?[0-9]+(\.[0-9]+)?/
    ;

ch : '\\' ("space"|"newline"|"return"|"tab"|"vtab")
   | '\\' /./
   ;

str : '"' /[^"]*/ '"' ;

bool : "True" | "False" ;

var : /[^() \t\r\n#`'"]+/ ;

ws : '#' /[^\n]*\n/ | /[ \t\r\n]*/ ;
