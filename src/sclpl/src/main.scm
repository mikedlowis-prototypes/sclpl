(declare (uses library))

(define primitive-types '(Num Char String))

(define (literal e)
  (if (not (or (number? e) (char? e) (string? e) (symbol? e)))
      (syn-error e "Not a recognized literal type")))

(define (prim-type e)
  (if (or (not (symbol? e))
          (not (member e primitive-types)))
    (syn-error e "Not a recognized primitive type")))

(define (typed-sym sym)
  (if (not (pair? sym))
      (syn-error sym "Expected a type/symbol pair"))
  (if (not (symbol? (car sym)))
      (syn-error sym "Expected a symbol name"))
  (prim-type (cdr sym)))

(define (list-of-types lst)
  (if (null? lst) lst
      (begin (typed-sym (car lst))
             (list-of-types (cdr lst)))))

(define (definition e)
  (if (or (not (list? e))
          (< (length e) 3))
    (syn-error e "Definitions require a type and a body or value"))
  (if (list? (cadr e)) (list-of-types (cadr e)) (typed-sym (cadr e))))



(define (syn-error expr msg)
  (print (string-append "Error: " msg))
  (display "\n")
  (pretty-print expr)
  (exit 1))

; Main
;------------------------------------------------------------------------------

