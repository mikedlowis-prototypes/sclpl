(declare (unit sclpl) (uses library))

(define (interpret port)
  (define expression (read port))
  (print (eval expression))
  (if (not (null? expression))
      (interpret port)))

