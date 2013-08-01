(require 'srfi-1)
; Type Inference Algorithm
;------------------------------------------------------------------------------
; This section implements algorithm W from the Luis Damas and Robin Milner
; "Principal type-schemes for functional program".
; The type inference rules have been extended to support more scheme-like
; semantics and uses.

(define type-env '())

(define (infer-type form)
  (set! type-env (env-empty))
  (call/cc
    (lambda (error-fn)
      (set! infer-error error-fn)
      (let [(type (algorithm-w (env-empty) form))]
           (substitute type-env type)))))

(define (algorithm-w env form)
    (cond [(symbol? form)         (get-var-type env form)]
          [(not (pair? form))     (get-const-type form)]
          [(eq? (car form) 'if)   (infer-cond-type env form)]
          [(eq? (car form) 'func) (infer-func-type env form)]
          [(eq? (car form) 'def)  (infer-def-type env form)]
          [else                   (infer-app-type env form)]))

(define (get-var-type env form)
  (define loc-val (env-value env form))
  (if loc-val
    (let [(kind (car loc-val))
          (type (cadr loc-val))]
      (if (eq? kind 'let)
          (instance env loc-val)
          type))
      (instance (env-empty) (env-value global-env form))))

(define (get-const-type form)
  (cond [(boolean? form) 'Bool]
        [(number? form)  'Num]
        [(char? form)    'Char]
        [(string? form)  'String]
        [else            (type-error form)]))

(define (infer-cond-type env form)
  (let [(cnd (algorithm-w env (cadr form)))
        (brt (algorithm-w env (caddr form)))
        (brf (algorithm-w env (cadddr form)))]
    (set! type-env (unify (unify env cnd 'Bool) brt brf))
    brt))

(define (infer-func-type env form)
  (let* [(parms   (map (lambda (x) (new-type-var)) (cadr form)))
         (new-env (env-join env (map (lambda (x y) (list x 'func y)) (cadr form) parms)))
         (ext-env (env-join new-env (get-local-var-env new-env (cddr form))))
         (body    (map (lambda (expr) (algorithm-w ext-env expr))
                       (get-func-body (cddr form))))]

    (cons '-> (append parms (list (car (reverse body)))))))

(define (get-local-var-env env body)
  (define defs (filter (lambda (a) (and (pair? a) (eq? (car a) 'def)))
                       body))
  (map (lambda (a) (list (cadr a) 'func (algorithm-w env (caddr a))))
       defs))

(define (get-func-body lst)
  (filter (lambda (a)
            (or (not (pair? a)) (not (eq? (car a) 'def))))
          lst))

(define (infer-def-type env form)
  (let [(name (cadr form))
        (type (algorithm-w env (caddr form)))]
    (if type (set! global-env (cons (cons name type) global-env)))
    type))

(define (infer-app-type env form)
  (let [(result (new-type-var))
        (oper   (algorithm-w env (car form)))
        (args   (map (lambda (x) (algorithm-w env x)) (cdr form)))]
    (set! type-env (unify type-env oper (cons '-> (append args (list result)))))
    result))

; Type Unification Algorithm
;------------------------------------------------------------------------------
; This section implements a type unification algorithm as described in
; J. A. Robinson's paper "A machine-oriented logic based on the resolution
; principle"

(define (unify env x y)
  (print env x y)
  (let [(x* (deref env x))
        (y* (deref env y))]
    (cond [(eq? x* y*)           env]
          [(var-and-type? x* y*) (env-set env x* y*)]
          [(var-and-type? y* x*) (env-set env y* x*)]
          [(and (pair? x*) (pair? y*))
           (unify (unify env (car x*) (car y*)) (cdr x*) (cdr y*))]
          [else                  (type-error x)])))

(define (var-and-type? var typ)
  (and (type-var? var)
       (or (not (type-var? typ))
           (type-var<? typ var))))

(define (deref env var)
  (if (and (type-var? var) (env-bound? env var))
      (deref env (env-value env var))
      var))

(define (substitute env x)
  (cond [(type-var? x) (let [(y (deref env x))]
                            (if (type-var? y) y (substitute env y)))]
        [(pair? x)     (cons (substitute env (car x))
                             (substitute env (cdr x)))]
        [else          x]))

(define (instance prefix x)   ; generate an instance of x with new variables
  (define (new x env success) ; in place of the generic variables in prefix
    (cond [(and (type-var? x) (generic? x prefix))
           (if (env-bound? x env)
             (success (env-value x env) env)
             (let ((var (new-type-var)))
               (success var (env-set env x var))))]
           [(pair? x)
            (new (car x) env
              (lambda (a env)
                (new (cdr x) env
                  (lambda (b env)
                    (success (cons a b) env)))))]
           [else
            (success x env)]))
  (new x (env-empty) (lambda (a env) a)))

(define (generic? var prefix)
  (cond [(null? prefix)           #t]
        [(eq? (cadar prefix) var) #f]
        [else                     (generic? var (cdr prefix))]))

; Environment Manipulation
;------------------------------------------------------------------------------
; This section defines a default environment and a variety of environment
; manipulation functions that will be used by the type inference algorithm.

(define global-env
  '(
     (+  . (-> Num Num Num))
     (-  . (-> Num Num Num))
     (*  . (-> Num Num Num))
     (/  . (-> Num Num Num))
     (<  . (-> Num Num Bool))
     (<= . (-> Num Num Bool))
     (=  . (-> Num Num Bool))
     (>= . (-> Num Num Bool))
   )
)

(define (env-empty) '())

(define (env-set env name type)
  (cons (cons name type) env))

(define (env-join env1 env2)
  (append env1 env2))

(define (env-bound? env name)
  (let [(type (assq name env))]
    (if type #t #f)))

(define (env-value env name)
  (let [(type (assq name env))]
    (if type (cdr type) #f)))

; Type Variable Creation and Usage
;------------------------------------------------------------------------------
; This section defines a series of functions for creating and comparing type
; variables to be used by the type inference algorithm.

(define (new-type-var)
  (set! type-var-count (+ type-var-count 1))
  (string->symbol (string-append "?" (number->string type-var-count))))

(define type-var-count 0)

(define (type-var? x)
  (and (symbol? x) (char=? (string-ref (symbol->string x) 0) #\?)))

(define (type-var<? x y)
  (string<? (symbol->string x) (symbol->string y)))

; Error Reporting
;------------------------------------------------------------------------------
; This section defines a series of functions for reporting specific type errors
; that occur during type inference.

(define infer-error (lambda (e) e))

(define (type-error form)
  (print "Type Error: Unable to determine type of expression")
  (infer-error #f))






;(print (infer-type '(+ 123 123)))
;(print (infer-type '(if #t 123 123)))
;(print "type" (infer-type '((func (a b) (+ a b)) #t 123)))

(print
  (infer-type
    '(func (a)
       (def b 123)
       (+ a b))))
;(exit)

