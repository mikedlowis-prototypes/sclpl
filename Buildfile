; Build Environment
;------------------------------------------------------------------------------
(environment BuildEnv
  '("CPPPATH" . ("source/libsof/"))
  '("LIBPATH" . ("build/")))

; Targets and Tasks
;------------------------------------------------------------------------------
(with-env BuildEnv
  (build "StaticLib" "build/sof"
    (glob "source/libsof/*.c"))

  (build "Program" "build/slvm"
    (glob "source/slvm/*.c"))

  (build "Program" "build/readsof"
    (glob "source/readsof/*.c")
    '("LIBS" . ("sof")))
  (depends "build/readsof" '("build/sof"))

  (build "Program" "build/slc"
    (glob "source/slc/*.scm"))

  (build "Program" "build/slpkg"
    (glob "source/slpkg/*.scm"))

  (build "Program" "build/slas"
    (glob "source/slas/*.scm"))
)

