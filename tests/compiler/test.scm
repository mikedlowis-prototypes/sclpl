(declare (unit test))

(define unit-tests '())

(define (register-test! test)
  (set! unit-tests (cons test unit-tests)))

(define (print-summary pass fail)
  (if (zero? fail)
    (print "Success: " pass " tests passed.")
    (print "Failure: " fail " / " (+ pass fail) " tests failed.")))

(define (run-all-unit-tests)
  (run-tests 0 0 (reverse unit-tests)))

(define (run-tests pass fail tests)
  (if (null? tests)
      (print-summary pass fail)
      (if (run-test (car tests))
          (run-tests (+ 1 pass) fail (cdr tests))
          (run-tests pass (+ 1 fail) (cdr tests)))))

(define (run-test test)
  (if (null? test)
      (error "Invalid test definition")
      (case (run-test-fn (cdr test))
        ('pass      #t)
        ('fail      (print "FAIL: "      (car test)) #f)
        ('error     (print "ERROR: "     (car test)) #f)
        ('exception (print "EXCEPTION: " (car test)) #f)
        (else       (print "UNKNOWN: "   (car test)) #f))))

(define (run-test-fn fn)
  (define preverr error)
  (define result
    (call/cc
      (lambda (cont)
        (set! error (lambda (x) (cont 'error)))
        (with-exception-handler
          (lambda (x) (cont 'exception))
          (lambda ()  (cont (if (fn) 'pass 'fail)))))))
  (set! error preverr)
  result)

