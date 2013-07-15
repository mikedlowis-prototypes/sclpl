(declare (unit desugar) (uses core-forms))

(define (desugar frm)
  (if (not (pair? frm)) frm
      (case (car frm)
            [(def)   (desugar-def frm)]
            [(begin) (desugar-begin frm)]
            [else    (map desugar frm)])))

(define (desugar-def frm)
  (if (and (>= (length frm) 3) (pair? (cadr frm)))
      (let* [(args* (cadr frm))
             (name  (car args*))
             (args  (cdr args*))]
        (list 'def name
          (append (list 'func args) (cddr frm))))
      (map desugar frm)))

(define (desugar-begin frm)
  (define defs (map desugar (cdr frm)))
  (if (not (list-of? defs definition?))
      (list (append (list 'lambda '()) defs))
      (cons 'begin defs)))

