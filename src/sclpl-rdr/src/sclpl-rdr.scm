(declare (unit sclpl-rdr) (uses library))

; Top-Level Interpret Function
;------------------------------------------------------------------------------
(define (interpret port)
  (define parsed-expr (expr port))
  (print parsed-expr)
  (if (not (null? parsed-expr))
      (interpret port)))

(define core-forms '("def" "set!" "if" "begin" "func"))

(define syntaxes   '())

; Expression Parsing Functions
;------------------------------------------------------------------------------
(define (expr port)
  (define tok (read-token port))
  (cond [(eof-object? tok) '()]
        [(core-form? tok)  (read-form-from-till port (string-append "sclpl/" tok) "end")]
        [(syntax? tok)     (read-form-from-till port tok "end")]
        [(equal? tok "{")  (cons 'sclpl/infix (read-form-till port "}"))]
        [(equal? tok "(")  (cons 'sclpl/list (read-form-till port ")"))]
        [(equal? tok "[")  (cons 'sclpl/prefix (read-form-till port "]"))]
        [else              (atom-or-fn-app port tok)]))

(define (core-form? tok)
  (member tok core-forms))

(define (syntax? tok)
  (member tok syntaxes))

(define (read-form-from-till port stok etok)
  (cons stok (read-form-till port etok)))

(define (read-form-till port etok)
  (define currexpr (expr port))
  (cond [(eof-object? currexpr)       (error "Unterminated expression")]
        [(and (string? currexpr)
              (equal? currexpr etok)) '()]
        [else                         (cons currexpr (read-form-till port etok))]))

(define (atom-or-fn-app port tok)
  (consume-ws-to-nl port)
  (if (char=? (peek-char port) #\()
      (cons 'sclpl/apply (cons tok (cdr (expr port))))
      tok))

(define (consume-ws-to-nl port)
  (if (and (char-whitespace? (peek-char port))
           (not (char=? (peek-char port) #\newline)))
      (begin (read-char port)
             (consume-ws-to-nl port))))

; Tokenizer Functions
;------------------------------------------------------------------------------
(define (read-token port)
  (define next (peek-char port))
  (cond [(eof-object? next)      (read-char port)]
        [(char-whitespace? next) (consume-whitespace port)]
        [(char=? next #\#)       (consume-comment port)]
        [(punctuation? next)     (punctuation port)]
        [else                    (read-atom port "")]))

(define (consume-whitespace port)
  (if (char-whitespace? (peek-char port))
      (begin (read-char port)
             (read-token port))
      (read-token port)))

(define (consume-comment port)
  (if (not (char=? #\newline (peek-char port)))
    (begin (read-char port) (consume-comment port))
    (begin (read-char port) (read-token port))))

(define (punctuation? ch)
  (and (not (eof-object? ch))
       (case ch ((#\( #\) #\{ #\}
                  #\[ #\] #\, #\;) #t)
                (else              #f))))

(define (punctuation port)
  (string (read-char port)))

(define (read-atom port str)
  (if (atom-char? (peek-char port))
      (read-atom port (string-append str (string (read-char port))))
      str))

(define (atom-char? ch)
  (and (not (eof-object? ch))
       (not (char-whitespace? ch))
       (not (punctuation? ch))))

