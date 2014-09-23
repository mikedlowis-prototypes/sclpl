(declare (uses posix))
(use posix)

; Task Definition and Interaction
;------------------------------------------------------------------------------
(define-record task name desc active? deps actions)

(define top-level-tasks '())

(define current-namespace
  (make-parameter '()))

(define current-desc
  (make-parameter #f))

(define (task-register! task)
  (define name  (task-name task))
  (define entry (assoc name top-level-tasks))
  (if (not entry)
    (set! top-level-tasks (cons (cons name task) top-level-tasks))
    (set-cdr! entry (task-merge (cdr entry) task))))

(define (task-merge task1 task2)
  (make-task (task-name task1) (task-desc task1) #t
             (append (task-deps task1)
                     (task-deps task2))
             (append (task-actions task1)
                     (task-actions task2))))

(define (task-lookup name)
  (define entry (assoc name top-level-tasks))
  (if (not entry)
    (error (string-append "No such task: " name))
    (cdr entry)))

(define (task-invoke! name)
  (define task (task-lookup name))
  (if (task-active? task)
    (begin (task-active?-set! task #f)
           (map task-invoke! (task-deps task))
           (map (lambda (fn) (fn)) (task-actions task)))))

(define (gen-task-name name)
  (define namespace (current-namespace))
  (if (not (null? namespace))
    (string-append namespace ":" name)
    name))

; Environment Functions
;------------------------------------------------------------------------------
(define-record builder defaults action)

(define (get-sys-env)
  (get-environment-variables))

(define (set-sys-env! newenv)
  (clear-sys-env!)
  (map (lambda (p) (setenv (car p) (cdr p)))
       newenv))

(define (clear-sys-env!)
  (map (lambda (p) (unsetenv (car p)))
       (get-environment-variables)))

(define current-env
  (let [(curr-env (get-sys-env))]
    (lambda args
      (if (> (length args) 0)
        (begin (set-sys-env! (car args))
               (set! curr-env (car args)))
        curr-env))))

(define (env-clone env . vars)
  (define newenv (map (lambda (p) (cons (car p) (cdr p))) env))
  (define newvals '())
  (map (lambda (e)
         (define entry (assoc (car e) newenv))
         (if entry
           (set-cdr! entry (cdr e))
           (set! newvals (cons e newvals))))
       vars)
  (append newvals newenv))

(define (env-get env key)
  (define entry (assoc key env))
  (if entry (cdr entry) #f))

(define (env-set env key value)
  (cons (cons key value)
        (env-unset env key)))

(define (env-unset env key)
  (cond [(null? env)               '()]
        [(string=? (caar env) key) (env-unset (cdr env) key)]
        [else                      (cons (car env) (env-unset (cdr env) key))]))

(define (env-extend env . vars)
  (foldl (lambda (env p)
           (env-set env (car p) (cdr p)))
         env
         vars))

(define (env-substitute env str)
  (list->string (sub-vars (string->list str) env)))

(define (env-prepend-path env path)
  '())

(define (env-append-path env path)
  '())

(define (env-add-builders env . builders)
  '())

; Builders
;------------------------------------------------------------------------------


; System Utility Functions
;------------------------------------------------------------------------------
(define verbose #f)

(define (build type . args)
  (define bldr (assoc type (assoc "builders" (current-env))))
  (define bldr-env (env-merge (builder-defaults bldr) (current-env)))
  (apply (builder-action bldr) (cons bldr-env args)))

(define (run . args)
  (define cmd (env-substitute (current-env) (string-join args " ")))
  (if verbose (print cmd))
  (if (not (= 0 (system cmd)))
    (fail-build cmd)))

(define (fail-build cmd)
  (print "Error: Command returned a non-zero status")
  (exit 1))

; Directories
(define cd change-directory)
(define curdir current-directory)
(define mkdir create-directory)
(define rmdir delete-directory)
(define lsdir directory)
(define dir? directory?)
; glob

; Files
(define cp '())
(define mv '())
(define rm delete-file)

; String Templating
;------------------------------------------------------------------------------
(define (sub-vars chlst env)
  (cond [(null? chlst)            '()]
        [(char=? #\$ (car chlst)) (let [(pair (replace-var (cdr chlst) env))]
                                    (append (string->list (car pair))
                                            (sub-vars (cdr pair) env)))]
        [else                     (cons (car chlst) (sub-vars (cdr chlst) env))]))

(define (replace-var chlst env)
  (define tok '())
  (define (collect-var-chars chlst)
    (if (or (null? chlst) (char=? (car chlst) #\space))
      (set! tok (cons (list->string (reverse tok)) chlst))
      (begin (set! tok (cons (car chlst) tok))
             (collect-var-chars (cdr chlst)))))
  (collect-var-chars chlst)
  (let [(var (env-get env (car tok)))]
    (if var
      (cons var (cdr tok))
      (cons ""  (cdr tok)))))

;(define (scan-tok chlst tok)
;  (cond [(or (null? chlst) (char=? #\space (car chlst)))
;         (list->string (reverse tok))]

; System Utility Functions
;------------------------------------------------------------------------------
(define (string-join strlst jstr)
  (foldl (lambda (a b) (string-append a b jstr)) "" strlst))

; DSL Definition
;------------------------------------------------------------------------------
(define-syntax task
  (syntax-rules (=>)
    [(_ name => (deps ...))
     (task-register! (make-task (gen-task-name name) (current-desc) #t '(deps ...) '()))]
    [(_ name => (deps ...) exp1 expn ...)
     (task-register!
       (make-task (gen-task-name name) (current-desc) #t '(deps ...)
         (list (lambda () exp1 expn ...))))]
    [(_ name exp1 expn ...)
     (task-register!
       (make-task (gen-task-name name) (current-desc) #t '()
         (list (lambda () exp1 expn ...))))]))

(define-syntax namespace
  (syntax-rules ()
    [(_ name body ...)
     (let [(prev-ns (current-namespace))]
       (current-namespace (gen-task-name name))
       body ...
       (current-namespace prev-ns))]))

(define (desc str)
  (current-desc str))

(define-syntax environment
  (syntax-rules (<=)
    [(_ name <= parent vars ...)
     (define name (env-extend parent vars ...))]
    [(_ name vars ...)
     (define name (env-extend (current-env) vars ...))]))

(define-syntax builder
  (syntax-rules (defaults action)
    [(_ (defaults vars ...) (action args body ...))
     (make-builder '(vars ...) (lambda args body ...))]))

; Core Tasks
;------------------------------------------------------------------------------
(task "verbose"
  (set! verbose #t))

(task "help"
  (map (lambda (t)
         (if (task-desc (cdr t))
           (print (string-append (task-name (cdr t)) " - " (task-desc (cdr t))))))
       top-level-tasks))
; Main
;------------------------------------------------------------------------------
(define (run-top-level-tasks!)
  (map task-invoke!
       (if (= 0 (length (command-line-arguments)))
         '("default")
         (command-line-arguments))))

(load "Spadefile")
(run-top-level-tasks!)

