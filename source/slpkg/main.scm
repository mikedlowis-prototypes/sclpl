(declare (uses library))

(define slpkg-usage
"Package manager for SCLPL (Simple Concurrent List Processing Language).

Usage:
  slpkg [COMMAND] [OPTIONS]

Commands:
  help           Show help documentation for a specific command or subcommand.
  install        Install one or more packages from the configured sources.
  publish        Publish a package to a specified repository.
  remove         Remove one or more packages from this machine.
  search         Search the repositories for packages matching a pattern.
  show           Show detailed information about a specific package or packages.
  source         Manage the sources from which packages will be retrieved.
  update         Update the package lists for all configured sources.
  upgrade        Upgrade a given package or packages.
  upgrade-all    Upgrade all packages installed on this machine.\n")

;------------------------------------------------------------------------------

(define (help-cmd args)
  (print args))

(define install-cmd help-cmd)
(define publish-cmd help-cmd)
(define remove-cmd help-cmd)
(define search-cmd help-cmd)
(define show-cmd help-cmd)
(define source-cmd help-cmd)
(define update-cmd help-cmd)
(define upgrade-cmd help-cmd)
(define upgrade-all help-cmd)

;------------------------------------------------------------------------------

(define slpkg-commands
  `(("help" .        ,help-cmd)
    ("install" .     ,install-cmd)
    ("publish" .     ,publish-cmd)
    ("remove" .      ,remove-cmd)
    ("search" .      ,search-cmd)
    ("show" .        ,show-cmd)
    ("source" .      ,source-cmd)
    ("update" .      ,update-cmd)
    ("upgrade" .     ,upgrade-cmd)
    ("upgrade-all" . ,upgrade-all)))

;------------------------------------------------------------------------------

(define (cmd-dispatch cmd-map usage args)
  (define sub-cmd (if (pair? args) (assoc (car args) cmd-map) '()))
  (cond [(pair? sub-cmd) ((cdr sub-cmd) (cdr args))]
        [else            (print usage)]))

;------------------------------------------------------------------------------

(cmd-dispatch slpkg-commands slpkg-usage (command-line-arguments))
