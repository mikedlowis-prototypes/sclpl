(declare (uses library eval core-forms desugar srfi-13 extras))

(define (compile-file fname)
  (define ofname (get-output-file-name fname))
  (define program (parse-file fname))
  (with-output-to-file ofname
    (lambda () (map pretty-print program)))
  (system (string-append "csc " ofname))
  (delete-file ofname))

(define (get-output-file-name ifname)
  (string-append (substring ifname 0 (string-index-right ifname #\.)) ".scm"))

(define (parse-file fname)
  (map core-form->scheme (read-forms (open-input-file fname))))

(define (read-forms port)
  (define form (desugar (read port)))
  (if (eof-object? form)
      '()
      (let [(errs (core-syntax-errors form))]
           (if (pair? errs) (begin (pprint-errors errs) (exit 1)))
           (cons form (read-forms port)))))

(define (core-form->scheme frm)
  (if (and (list? frm) (not (null? frm)))
      (case (car frm)
            [(def)  (cons 'define (map core-form->scheme (cdr frm)))]
            [(func) (cons 'lambda (map core-form->scheme (cdr frm)))]
            [else   (map core-form->scheme frm)])
      frm))

(define (pprint-errors elst)
  (if (pair? elst)
      (begin (print "Error: " (cdar elst))
             (pretty-print (caar elst))
             (pprint-errors (cdr elst)))))

; If we have a file, then parse it
(if (= 1 (length (command-line-arguments)))
  (compile-file (car (command-line-arguments)))
  (print "No input file provided."))

