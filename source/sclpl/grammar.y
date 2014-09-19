
token : /^/ <atom> /$/ ;

atom : <punc> | <radixnum> | <floating> | <integer> | <character> | <boolean> | <var> ;

punc : /['"(){}\[\];,]/ ;

floating : /[-+]?[0-9]+\.[0-9]+/ ;

integer : /[-+]?[0-9]+/ ;

radixnum : "0b" /[0-1]+/
         | "0o" /[0-7]+/
         | "0d" /[0-9]+/
         | "0x" /[0-9a-fA-F]+/
         ;

character : '\\' ("space"|"newline"|"return"|"tab"|"vtab")
          | '\\' /./
          ;

boolean : "true" | "false" ;

var : /.*/ ;
