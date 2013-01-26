(declare (uses library))

; Grammar Creation Procedures
;------------------------------------------------------------------------------
(define (patt-matches? e patt)
  (print "patt-matches? " e " " patt)
  (cond [(procedure? patt) (patt e)]
        [(list? patt)      (patt-list-matches? e patt)]
        [(symbol? patt)    (equal? e patt)]
        [else              (error "Malformed pattern detected")]))

(define (patt-list-matches? e patt)
  (print "patt-list-matches? " e " " patt)
  (cond [(and (null? patt) (null? e))         #t]
        [(null? e)                            #f]
        [(null? patt)                         #f]
        [(patt-matches? (car e) (car patt))   (patt-list-matches? (cdr e) (cdr patt))]
        [else                                 #f]))

(define (patt-matches-one-of? e lpatts)
  (print "patt-matches-one-of? " e " " lpatts)
  (if (null? lpatts) #f
      (if (patt-matches? e (car lpatts)) #t
          (patt-matches-one-of? e (cdr lpatts)))))

(define (n-of proc)
  (define (match-fn e)
    (and (list? e)
         (or (null? e)
             (and (proc (car e))
                  (match-fn (cdr e))))))
  match-fn)

; Helper Macros
;------------------------------------------------------------------------------
(define-syntax rule
  (syntax-rules (:)
    [(_ name : patt ...) (define (name e)
                           (patt-matches-one-of? e (list patt ...)))]))

(define-syntax grammar
  (syntax-rules (:> :)
    [(_ name :> start patts ...) (define (name e) patts ... (start e))]))

; Grammar
;------------------------------------------------------------------------------
(grammar SCLPL :> Expression
  (rule Expression : Literal
                     Conditional
                     Definition)

  (rule Literal : number?
                  char?
                  string?
                  symbol?)

  (rule Conditional : `(if ,Expression)
                      `(if ,Expression ,Expression))

  (rule Definition : `(def ,TypePair ,Expression)
                     `(def ,TypePairList ,ExpList))

  (rule ExpList : (n-of Expression))
  (rule TypePairList : (n-of TypePair))
  (rule TypePair : `(,Id : ,PrimType))
  (rule Id : symbol?)
  (rule PrimType : prim-type?))

; Grammar Predicates
;------------------------------------------------------------------------------
(define (prim-type? e)
  (if (member e '(Num Char String Sym)) #t #f))

; Grammar Actions
;------------------------------------------------------------------------------

; Main
;------------------------------------------------------------------------------
