(declare (uses library eval core-forms))

(define (start-repl prt)
  (display ":> ")
  (let [(form (read prt))]
       (validate-form form)
       (print (eval (core-form->scheme form)
              (interaction-environment))))
  (start-repl prt))

(define (core-form->scheme frm)
  (if (and (list? frm) (not (null? frm)))
      (case (car frm)
            [(def)  (cons 'define (map core-form->scheme (cdr frm)))]
            [(func) (cons 'lambda (map core-form->scheme (cdr frm)))]
            [else   (map core-form->scheme frm)])
      frm))

(start-repl (current-input-port))

