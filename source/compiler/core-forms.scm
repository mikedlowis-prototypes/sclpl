(declare (unit core-forms) (uses srfi-1))

; Formal Syntax
;------------------------------------------------------------------------------
;
; Program ::= Form*
;
; Form ::= Definition | Expression
;
; Definition ::= '(def' Variable Expression ')'
;              | '(begin' Definition* ')'
;
; Expression ::= Constant
;              | Variable
;              | '(quote' Datum ')'
;              | '(func' ArgList Expression Expression* ')'
;              | '(if' Expression Expression Expression ')'
;              | '(set!' Variable Expression ')'
;              | '(' Expression Expression* ')'
;
; Constant ::= Boolean
;            | Number
;            | Character
;            | String
;
; ArgList ::= '(' Variable ')'
;           | '(' Variable Variable '.' Variable ')'
;

(define (validate-form frm)
  (if (definition? frm)
      (validate-definition frm)
      (validate-expression frm)))

(define (validate-definition frm)
  (if (and (list? frm) (not (null? frm)))
      (case (car frm)
            [(begin) (map validate-definition (cdr frm))]
            [(def)   (if (not (def? frm))
                         (syntx-err frm "Not a valid def expression"))]
            [else    (syntx-err frm "Not a valid definition")])
      (syntx-err frm "Not a valid definition")))

(define (validate-expression frm)
  (if (and (list? frm) (not (null? frm)))
      (case (car frm)
            [(quote) (validate-quotation frm)]
            [(func)  (validate-func frm)]
            [(if)    (validate-if frm)]
            [(set!)   (validate-set frm)]
            [else    (validate-apply frm)])
      (validate-constant frm)))

(define (validate-quotation frm)
  (if (not (quote? frm))
      (syntx-err frm "Invalid quotation")))

(define (validate-func frm)
  (if (not (func? frm))
      (syntx-err frm "Invalid function definition")))

(define (validate-if frm)
  (if (not (if? frm))
      (syntx-err frm "Invalid if statement")))

(define (validate-set frm)
  (if (not (set? frm))
      (syntx-err frm "Invalid assignment expression")))

(define (validate-apply frm)
  (if (not (apply? frm))
      (syntx-err frm "Invalid application expression")))

(define (validate-constant frm)
  (if (not (constant? frm))
      (syntx-err frm "Not a valid constant")))

;------------------------------------------------------------------------------

(define (form? frm)
  (or (definition? frm) (expression? frm)))

(define (definition? frm)
  (or (def? frm) (begin? frm)))

(define (def? frm)
  (and (list-of-length? frm 3)
       (equal? 'def (car frm))
       (symbol? (cadr frm))
       (expression? (caddr frm))))

(define (begin? frm)
  (and (list-of->=-length? frm 2)
       (equal? 'begin (car frm))
       (list-of? (cdr frm) definition?)))

(define (expression? expr)
  (if (constant? expr) #t
      (and (list? expr)
           (not (null? expr))
           (case (car expr) [(begin def) #f]
                            [else        #t]))))

(define (quote? frm)
  (and (list-of-length? frm 2)
       (equal? 'quote (car frm))))

(define (func? frm)
  (and (list-of->=-length? frm 3)
       (equal? 'func (car frm))
       (list-of? (cadr frm) symbol?)
       (list-of? (cddr frm) expression?)))

(define (if? frm)
  (and (list-of-length? frm 4)
       (equal? 'if (car frm))
       (list-of? (cdr frm) expression?)))

(define (set? frm)
  (and (list-of-length? frm 3)
       (equal? 'set! (car frm))
       (symbol? (cadr frm))
       (expression? (caddr frm))))

(define (apply? frm)
  (and (not (null? frm))
       (list-of? frm expression?)))

(define (constant? frm)
  (or (number? frm) (string? frm) (symbol? frm) (char? frm) (boolean? frm)))

; Utility Predicate Procedures
;------------------------------------------------------------------------------
(define (list-of? lst prdfn)
  (if (not (list? lst)) #f
      (if (null? lst) #t
          (and (prdfn (car lst))
               (list-of? (cdr lst) prdfn)))))

(define (list-of->=-length? lst len)
  (and (list? lst) (>= (length lst) len)))

(define (list-of-length? lst len)
  (and (list? lst) (= (length lst) len)))

(define (get-free-vars expr)
  (if (symbol? expr) expr
      (case (if (list? expr) (car expr) '())
            [(def set!) (list (cadr expr) (get-free-vars (caddr expr)))]
            [(begin if) (map get-free-vars (cdr expr))]
            [(func)     (filter-vars (cadr expr) (map get-free-vars (cddr expr)))]
            [else       (map get-free-vars expr)])))

(define (filter-vars defd lst)
  (filter (lambda (item) (not (member item defd))) lst))

; Utility Procedures
;------------------------------------------------------------------------------
(define (syntx-err frm msg)
  (print "Error: " msg "\n")
  (pretty-print frm)
  (display "\n")
  (exit 1))

