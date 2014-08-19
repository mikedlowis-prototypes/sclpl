
program : /^/ <expr> /$/ ;

replexpr : <ws> <expr> /[^\n]*\n/ ;

expr : <sexpr> | <qexpr> | <radixnum> | <float> | <int> | <ch> | <str> | <bool> | <var> ;

sexpr : '(' (<ws> <expr> <ws>)* ')' ;

qexpr : ('\'' | '`' | ',') <expr> ;

atom : <float> | <int> | <radixnum> | <ch> | <str> | <bool> | <var> ;

int : /[-+]?[0-9]+/ ;

float : /[-+]?[0-9]+\.[0-9]+/ ;

radixnum : "0b" /[0-1]+/
         | "0o" /[0-7]+/
         | "0d" /[0-9]+/
         | "0x" /[0-9a-fA-F]+/
         ;

ch : '\\' ("space"|"newline"|"return"|"tab"|"vtab")
   | '\\' /./
   ;

str : '"' /[^"]*/ '"' ;

bool : "True" | "False" ;

var : /[^() \t\r\n#`'"]+/ ;

ws : '#' /[^\n]*\n/ | /[ \t\r\n]*/ ;