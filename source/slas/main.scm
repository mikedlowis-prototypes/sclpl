(declare (uses library))

(define usage
"\nUsage: slas <INFILE> <OUTFILE>

Assemble <INFILE> to SCLPL bytecode and write the result to <OUTFILE>.\n")

; Control Routines
;------------------------------------------------------------------------------
(define (assemble-file infile outfile)
  (define iprt (open-input-file infile))
  (define oprt (open-output-file outfile))
  (generate-bytecode iprt oprt))

(define (generate-bytecode iprt oprt) '())

; Main routine
;------------------------------------------------------------------------------
(if (= 2 (length (command-line-arguments)))
    (apply assemble-file (command-line-arguments))
    (print usage))
(exit)
