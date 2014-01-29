; Regex Matching Macro
;------------------------------------------------------------------------------
(use regex ports extras)

(define-syntax regex-case
  (syntax-rules (else)
    ((_ item (else result1 result2 ...))
     (begin result1 result2 ...))

    ((_ item (regex result1 result2 ...))
     (if (string-match regex item) (begin result1 result2 ...)))

    ((_ item (regex result1 result2 ...) clause1 clause2 ...)
     (if (string-match regex item)
       (begin result1 result2 ...)
       (regex-case item clause1 clause2 ...)))))

; Reader Phase
;------------------------------------------------------------------------------
; This phase is responsible reading input from a port and constructing the
; expression that the input represents.

(define (sclpl-read port)
  (let [(tok (read-token port))]
    (if (eof-object? tok)
      tok
      (cond [(list-op? tok)    (read-sexp port (get-sexp-term tok))]
            [(equal? "'" tok)  `(quote ,(sclpl-read port))]
            [(equal? "`" tok)  `(quasiquote ,(sclpl-read port))]
            [else              (classify-atom tok)]))))

(define (read-sexp port term)
  (define expr (sclpl-read port))
  (cond [(equal? expr term)      '()]
        [(wrong-term? expr term) (error "Incorrectly matched list terminator")]
        [(equal? '|.| expr)      (read-and-term port term)]
        [else                    (cons expr (read-sexp port term))]))


(define (read-and-term port term)
  (define val  (sclpl-read port))
  (define tval (sclpl-read port))
  (cond [(member val '(#\) #\] #\})) (error "")]
        [(equal? tval term)          val]
        [(wrong-term? tval term)     (error "")]
        [else                        (error "")]))

(define (classify-atom atom)
  (regex-case atom
    ["nil"               '()]
    ["true"              #t]
    ["false"             #f]
    ["^\".*\"$"          (dequote atom)]
    ["^\\\\.+"           (atom->char atom)]
    ["[{[()\\]}]"        (string-ref atom 0)]
    ["#[dbox](#[ie])?.+" (or (string->number atom) (string->symbol atom))]
    ["[+-]?[0-9].*"      (or (string->number atom) (string->symbol atom))]
    [else                (if (string-literal? atom)
                           (dequote atom)
                           (string->symbol atom))]))

(define (list-op? tok)
  (member (string-ref tok 0) '(#\( #\[ #\{)))

(define (get-sexp-term tok)
  (define pairs '((#\( . #\)) (#\[ . #\]) (#\{ . #\})))
  (define term  (assv (string-ref tok 0) pairs))
  (if term (cdr term) (error "Not a valid s-expression delimiter")))

(define (wrong-term? expr term)
  (define terms '(#\) #\] #\}))
  (and (not (equal? expr term))
       (member expr terms)))

(define (string-literal? atom)
  (and (char=? #\" (string-ref atom 0))
       (char=? #\" (string-ref atom (- (string-length atom) 1)))))

(define (dequote str)
  (substring str 1 (- (string-length str) 1)))

(define (atom->char atom)
  (define ch-name (substring atom 1))
  (define ch (if (= 1 (string-length ch-name))
               (string-ref ch-name 0)
               (char-name (string->symbol ch-name))))
  (or ch (error (string-append "Invalid character name: " ch-name))))

;------------------------------------------------------------------------------

(define whitespace  (string->list " \t\r\n"))
(define punctuation (string->list "()[]{}'`:,"))
(define delimiters  (string->list "()[]{}'`:,; \t\r\n"))
(define doublequote '(#\"))

(define (read-token port)
  (define ch (peek-char port))
  (define tok
    (cond [(eof-object? ch)        ch]
          [(member ch whitespace)  (consume-whitespace port)]
          [(char=? ch #\;)         (consume-comment port)]
          [(char=? ch #\")         (read-till-next #t port doublequote)]
          [(member ch punctuation) (string (read-char port))]
          [else                    (read-till-next #f port delimiters)]))
  (if (list? tok) (list->string tok) tok))

(define (consume-whitespace port)
  (if (member (peek-char port) whitespace)
    (read-char port))
  (read-token port))

(define (consume-comment port)
  (if (not (char=? #\newline (peek-char port)))
    (begin (read-char port)
           (consume-comment port))
    (begin (read-char port)
           (read-token port))))

(define (read-till-next inc port delims)
  (cons (read-char port)
        (if (or (member (peek-char port) delims)
                (eof-object? (peek-char port)))
          (if inc (cons (read-char port) '()) '())
          (read-till-next inc port delims))))

; Macro Expansion Phase
;------------------------------------------------------------------------------
; This phase is responsible for taking the expressions read from the input port
; and performing macro expansion on them to get the resulting expression.

(define (expand-macros expr)
  expr)

; Desugaring Phase
;------------------------------------------------------------------------------
; The desugaring phase is responsible for taking user friendly extensions to
; the core SCLPL syntax and deconstructing them into the low-level counterparts
; defined by the "core" SCLPL syntax. This allows the code generator to work on
; a small and well-defined subset of the SCLPL language.

(define (desugar expr)
  (cond [(not (pair? expr))     expr]
        [(eqv? 'def (car expr)) (desugar-def expr)]
        [(eqv? 'if (car expr))  (desugar-if expr)]
        [(eqv? 'fn (car expr))  (append (list 'fn (cadr expr))
                                        (map desugar (cddr expr)))]
        [else                   (map desugar expr)]))

(define (desugar-def expr)
  (cond [(annotated-def? expr) (desugar-annotated-def expr)]
        [(sugared-def? expr)   (desugar-sugared-def expr)]
        [else                  (map desugar expr)]))

(define (annotated-def? expr)
  (and (form-structure-valid? 'def >= 4 expr)
       (eqv? ': (caddr expr))))

(define (sugared-def? expr)
  (and (form-structure-valid? 'def >= 2 expr)
       (arg-list-valid? (cadr expr))))

(define (desugar-annotated-def expr)
  (let [(proto (cadr expr))
        (type  (cadddr expr))
        (body  (cddddr expr))]
    (if (pair? proto)
      (append `(def (,(car proto) ,type))
              (list (append `(fn ,(cdr proto)) (map desugar body))))
      (append `(def (,proto ,type))
              (map desugar body)))))

(define (desugar-sugared-def expr)
  (if (pair? (cadr expr))
    (append `(def (,(caadr expr) ()))
            (list (append `(fn ,(cdadr expr)) (map desugar (cddr expr)))))
    (append `(def (,(cadr expr) ())) (map desugar (cddr expr)))))

(define (desugar-if expr)
  (if (form-structure-valid? 'if = 3 expr)
    (map desugar (append expr '('())))
    (map desugar expr)))

; Analysis Phase
;------------------------------------------------------------------------------
; The analysis phase is responsible for verifying that the provided expression
; conforms to the requirements of the "core" SCLPL syntax. This phase will throw
; an error for any invalid expression or simply return the provided expression
; if it is valid.

(define (analyze expr)
  (if (list? expr)
    (analyze-form expr)
    expr))

(define (analyze-form expr)
  (if (null? expr)
    (error-msg 'non-atomic expr)
    (case (car expr)
      [(def)   (analyze-def expr)]
      [(fn)    (analyze-fn expr)]
      [(if)    (validate-and-analyze 'if    =  4 expr)]
      [(do)    (validate-and-analyze 'do    >= 1 expr)]
      [(quote) (validate-and-analyze 'quote =  2 expr)]
      [else    (map analyze expr)])))

(define (analyze-def expr)
  (validate-form 'def = 3 expr)
  (validate-signature (cadr expr))
  expr)

(define (analyze-fn expr)
  (if (and (form-structure-valid? 'fn >= 3 expr)
           (arg-list-valid? (cadr expr)))
    (append (list 'fn (cadr expr))
            (map analyze (cddr expr)))
    (error-msg 'invalid-fn expr)))

(define (validate-and-analyze type cmpop nargs expr)
  (validate-form type cmpop nargs expr)
  (map analyze expr))

(define (validate-form type cmpop nargs expr)
  (cond [(not (pair? expr))                (error-msg 'not-an-sexp expr)]
        [(not (eqv? type (car expr)))      (error-msg 'wrong-form-type expr)]
        [(not (cmpop (length expr) nargs)) (error-msg 'num-args expr)]))

(define (validate-signature sig)
  (cond [(not (list? sig))               (error-msg 'sig-not-list sig)]
        [(not (= 2 (length sig)))        (error-msg 'sig-num-entries sig)]
        [(not (variable? (car sig)))     (error-msg 'sig-variable sig)]
        ;[(not (type? (cadr sig)))        (error-msg 'expect-type sig)]))
        ))

; Type Checking Phase
;------------------------------------------------------------------------------
; This phase is responsible for performing type reconstruction and verifying
; that the expression is well-typed before being passed to the optimization and
; compilation phases

(define (check-type expr env)
  expr)

; CPS-Conversion Phase
;------------------------------------------------------------------------------
; This phase translates the fully macro-expanded, desugared, and analyzed
; program into continuation-passing style so various optimizations can be
; performed before code is generated.

(define (cps-convert expr)
  expr)

; SCLPL to Scheme Phase
;------------------------------------------------------------------------------

(define (sclpl->scheme expr)
  expr)

; Error Messages
;------------------------------------------------------------------------------
(define (error-msg type expr . args)
  (let [(handler (assoc type error-handlers))]
    (if handler (apply (cdr handler) args) (apply unknown-error args))
    (log-msg (with-output-to-string (lambda () (pretty-print expr))))
    (fail expr)))

(define (non-atomic-expr)
  (log-msg "Error: Illegal non-atomic object"))

(define (invalid-fn)
  (log-msg "Error: Invalid function form"))

(define (not-an-sexpr)
  (log-msg "Error: Not an s-expression"))

(define (wrong-form-type)
  (log-msg "Error: Incorrect form type"))

(define (wrong-num-args)
  (log-msg "Error: Incorrect number of args for form"))

(define (sig-is-not-a-list)
  (log-msg "Error: Function signature is not a list"))

(define (wrong-num-sig-parts)
  (log-msg "Error: Function signature has incorrect number of parts"))

(define (sig-name-not-var)
  (log-msg "Error: Name part of function signature is not a variable"))

(define (expected-:)
  (log-msg "Error: Expected a :"))

(define (unknown-error . args)
  (log-msg "Error: Unknown error occurred in the following expression"))

(define error-handlers
 `((non-atomic      . ,non-atomic-expr)
   (invalid-fn      . ,invalid-fn)
   (not-an-sexp     . ,not-an-sexpr)
   (wrong-form-type . ,wrong-form-type)
   (num-args        . ,wrong-num-args)
   (sig-not-list    . ,sig-is-not-a-list)
   (sig-num-entries . ,wrong-num-sig-parts)
   (sig-variable    . ,sig-name-not-var)
   (expect-:        . ,expected-:)))

; Helper Predicates
;------------------------------------------------------------------------------
; This collection of predicate functions is used to assist the earlier phases
; when dealing with similar data-structures.

(define (form-structure-valid? type cmpop nargs expr)
  (and (pair? expr)
       (eqv? type (car expr))
       (cmpop (length expr) nargs)))

(define (arg-list-valid? arglst)
  (or (variable? arglst)
      (and (or (list? arglst) (pair? arglst))
           (list-of? variable? arglst))))

(define (list-of? type lst)
  (if (null? lst) #t
    (if (type (car lst))
      (if (type (cdr lst)) #t (list-of? type (cdr lst)))
      #f)))

(define (variable? sym)
  (and (symbol? sym)
       (not (type-name? sym))
       (not (type-var? sym))))

(define (atomic-base-type? type)
  (if (member type '(Any Number Symbol String Char Bool)) #t #f))

(define (type? expr)
  (cond [(null? expr)      #f]
        [(member '-> expr) (fn-type? expr)]
        [(list? expr)      (and (> (length expr) 1)
                                (apply list-and? (map type? expr)))]
        [else              (or (type-name? expr) (type-var? expr))]))

(define (type-name? sym)
  (and (symbol? sym)
       (let [(ch (string-ref (symbol->string sym) 0))]
         (and (char>=? ch #\A) (char<=? ch #\Z)))))

(define (type-var? sym)
  (and (symbol? sym)
       (let [(ch (string-ref (symbol->string sym) 0))]
         (char=? ch #\?))))

(define (fn-type? expr)
  (define (is-fn-type? prev expr)
    (if (null? expr) #t
      (case (car expr)
        [(...) (and (type? prev)
                    (>= (length (cdr expr)) 1)
                    (equal? '-> (cadr expr))
                    (is-fn-type? #f (cdr expr)))]
        [(->)  (and (= 2 (length expr))
                    (is-fn-type? (car expr) (cdr expr)))]
        [else  (and (type? (car expr))
                    (is-fn-type? (car expr) (cdr expr)))])))
  (is-fn-type? #f expr))

(define (list-and? . args)
  (if (null? args) #t (and (car args) (apply list-and? (cdr args)))))

; Main
;------------------------------------------------------------------------------

(define fail error)

(define log-msg (lambda args '()))

;(define (print-data . args)
;  (apply print
;         (map (lambda (e)
;                (if (string? e)
;                  e
;                  (with-output-to-string (lambda () (pretty-print e)))))
;              args)))

;(define (interpret port)
;  (call/cc (lambda (k)
;    (set! fail k)
;    (set! log-msg print)
;    (display (string-append ":" (sexp-count) "> "))
;    ; Read and type and analyze all expressions from input
;    (define expr (sclpl-read port))
;    (print-data "Read Phase: \n" expr)
;    (set!   expr (expand-macros expr))
;    (print-data "Macro Expansion Phase: \n" expr)
;    (set!   expr (desugar expr))
;    (print-data "Desugar Phase: \n" expr)
;    (set!   expr (analyze expr))
;    (print-data "Analysis Phase: \n" expr)))
;  (interpret port))
;(interpret (current-input-port))
;(exit)

(define (read-program port)
  (define expr (sclpl-read port))
  (if (eof-object? expr)
    '()
    (cons (analyze (desugar (expand-macros expr)))
          (read-program port))))

(print (read-program (current-input-port)))


