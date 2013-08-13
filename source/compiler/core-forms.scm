(declare (unit core-forms) (uses srfi-1))

; Formal Syntax
;------------------------------------------------------------------------------
;
; Program ::= Form*
;
; Form ::= Definition | Expression
;
; Definition ::= '(def' Variable Expression ')'
;              | TypeAnnotation
;              | TypeDefinition
;              | '(begin' Definition* ')'
;
; TypeAnnotation ::= '(def:' Variable TypeConstructor ')'
;
; TypeConstructor ::= Variable
;                   | '(' Variable+ ')'
;
; TypeDefinition ::= '(data' Variable TypeConstructor+ TypeClassList? ')'
;                  | '(type' Variable Variable ')'
;                  | '(class' Variable OpAnnotation+ ')'
;                  | '(instance' Variable OpDefinition+ ')'
;
; TypeClassList ::= '(deriving' Variable+ ')'
;
; OpAnnotation ::= '(' Variable TypeConstructor ')'
;
; OpDefinition ::= '(' TypeConstructor Expression+ ')'
;
; Expression ::= Constant
;              | Variable
;              | '(quote' Datum ')'
;              | '(func' ArgList Expression+ ')'
;              | '(if' Expression Expression Expression ')'
;              | '(set!' Variable Expression ')'
;              | '(' Expression+ ')'
;
; Constant ::= Boolean | Number | Character | String
;
; ArgList ::= '(' Variable ')'
;           | '(' Variable Variable '.' Variable ')'
;

(define (core-syntax-errors frm)
  (if (definition? frm)
      (definition-errors frm)
      (expr-errors frm)))

(define (definition-errors frm)
  (case (if (pair? frm) (car frm) '())
        [(def)   (def-errors frm)]
        [(def:)  (annotation-errors frm)]
        [(begin) (begin-errors frm)]
        [else    (list (cons frm 'not-a-def-or-begin))]))

(define (def-errors frm)
  (cond [(not (pair? frm))          (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'def)) (list (cons frm 'not-a-def))]
        [(< (length frm) 2)         (list (cons frm 'missing-name))]
        [(< (length frm) 3)         (list (cons frm 'missing-value))]
        [(> (length frm) 3)         (list (cons frm 'too-many-items))]
        [else                       (if (not (symbol? (cadr frm)))
                                        (list (cons frm 'def-needs-symbol))
                                        (expr-errors (cddr frm)))]))
(define (annotation-errors frm)
  (cond [(not (pair? frm))           (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'def:)) (list (cons frm 'not-an-anno))]
        [(< (length frm) 2)          (list (cons frm 'missing-name))]
        [(< (length frm) 3)          (list (cons frm 'missing-type))]
        [else                        (if (not (symbol? (cadr frm)))
                                         (list (cons frm 'def-needs-symbol))
                                         (type-errors (caddr frm)))]))

(define (begin-errors frm)
  (cond [(not (pair? frm))            (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'begin)) (list (cons frm 'not-a-begin))]
        [(< (length frm) 2)           (list (cons frm 'no-defs))]
        [else                         (def-list-errors (cdr frm))]))

(define (expr-errors frm)
  (if (pair? frm)
      (case (car frm)
            [(quote) (quote-errors frm)]
            [(func)  (func-errors frm)]
            [(if)    (if-errors frm)]
            [(set!)  (set!-errors frm)]
            [else    (app-errors frm)])
      (const-errors frm)))

(define (quote-errors frm)
  (cond [(not (pair? frm))            (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'quote)) (list (cons frm 'not-a-quote))]
        [(< (length frm) 2)           (list (cons frm 'no-datum))]
        [(> (length frm) 2)           (list (cons frm 'too-many-items))]
        [else                         '()]))

(define (func-errors frm)
  (cond [(not (pair? frm))           (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'func)) (list (cons frm 'not-a-func))]
        [(< (length frm) 2)          (list (cons frm 'no-args-list))]
        [(< (length frm) 3)          (list (cons frm 'no-body))]
        [else                        (append (args-errors (cadr frm))
                                             (exp-list-errors (cddr frm)))]))

(define (if-errors frm)
  (cond [(not (pair? frm))         (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'if)) (list (cons frm 'not-an-if))]
        [(< (length frm) 2)        (list (cons frm 'missing-cond))]
        [(< (length frm) 3)        (list (cons frm 'too-few-branches))]
        [(> (length frm) 4)        (list (cons frm 'too-many-branches))]
        [else                      (exp-list-errors (cdr frm))]))

(define (set!-errors frm)
  (cond [(not (pair? frm))           (list (cons frm 'not-a-form))]
        [(not (eq? (car frm) 'set!)) (list (cons frm 'not-a-set!))]
        [(< (length frm) 2)          (list (cons frm 'missing-name))]
        [(< (length frm) 3)          (list (cons frm 'missing-value))]
        [(> (length frm) 3)          (list (cons frm 'too-many-items))]
        [else                        (exp-list-errors (cdr frm))]))

(define (app-errors frm)
  (exp-list-errors frm))

(define (const-errors frm)
  (if (or (boolean? frm) (number? frm) (char? frm) (string? frm) (symbol? frm))
      '()
      (list (cons frm 'not-a-const ))))

(define (args-errors frm)
  (if (or (null? frm) (lst-of? frm symbol?))
      '()
      (list (cons frm 'malformed-args))))

(define (def-list-errors dlst)
  (apply append (map definition-errors dlst)))

(define (exp-list-errors elst)
  (apply append (map core-syntax-errors elst)))

(define (type-errors typ)
  (if (or (symbol? typ) (lst-of? typ symbol?))
      '()
      (list (cons typ 'not-a-type))))

;------------------------------------------------------------------------------

(define (lst-of? lst prdfn)
  (if (and (pair? lst)
           (prdfn (car lst))
           (if (null? (cdr lst)) #t (lst-of? (cdr lst) prdfn)))
      #t #f))

(define (form-of-type? frm type)
  (and (pair? frm) (eq? (car frm) type)))

(define (definition? frm)
  (or (form-of-type? frm 'def)
      (form-of-type? frm 'def:)
      (form-of-type? frm 'begin)))

;------------------------------------------------------------------------------

(define error-msgs
  '((not-a-def . "Not a recognized definition form")
    (not-a-def . "Not a recognized definition form")
  )
)

;(define (repl port)
;  (display ":> ")
;  (pretty-print (core-syntax-errors (read port)))
;  (repl port))
;
;(repl (current-input-port))
