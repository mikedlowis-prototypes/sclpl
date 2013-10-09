#------------------------------------------------------------------------------
# Environment Setup and Utilities
#------------------------------------------------------------------------------

import platform
import fnmatch
import os

# Helper function for recursively finding files
def find_files(path,pattern):
    matches = []
    for root, dirs, files in os.walk(path):
        for filename in fnmatch.filter(files, pattern):
            matches.append(os.path.join(root, filename))
    return matches

#------------------------------------------------------------------------------
# Compiler Environments
#------------------------------------------------------------------------------

# Default C/C++ Environment
#---------------------------
c_cpp = Environment(
        ENV      = os.environ,
        CCFLAGS  = [ '-Wall', '-Werror', '-std=c99' ],
        LDFLAGS  = [],
        TOOLS = [ 'mingw' ])

# Chicken Scheme Environment
#---------------------------
# Scheme Source Compiler
scheme_compiler = Builder(
        action        = 'csc $CCFLAGS -c -o $TARGET $SOURCE',
        suffix        = '.o',
        src_suffix    = '.scm',
        single_source = True)

# Scheme Binary Linker
scheme_linker = Builder(
        action      = 'csc $LDFLAGS -o $TARGET $SOURCES',
        suffix      = "$PROGSUFFIX",
        src_suffix  = '.o',
        src_builder = [ scheme_compiler ])

# Scheme Test Linker
scheme_tester = Builder(
        action      = 'csc $LDFLAGS -o $TARGET $SOURCES && $TARGET',
        suffix      = "$PROGSUFFIX",
        src_suffix  = '.o',
        src_builder = [ scheme_compiler ])

# Create the Environment for this project
scheme = Environment(
        ENV      = os.environ,
        CCFLAGS  = [ '-explicit-use', '-I', 'inc'],
        LDFLAGS  = [],
        BUILDERS = {
            'Program': scheme_linker,
            'TestRunner': scheme_tester })

#------------------------------------------------------------------------------
# SCLPL Targets
#------------------------------------------------------------------------------

# SOF Shared Library
c_cpp.SharedLibrary(
        target = 'build/sof',
        source = find_files('source/libsof/','*.c'))

# SBC Shared Library
c_cpp.SharedLibrary(
        target = 'build/sbc',
        source = find_files('source/libsbc/','*.c'))

# readsof Command Line Utility
readsof = c_cpp.Clone(
        CPPPATH = [ 'source/libsof/' ],
        LIBS = [ 'sof' ],
        LIBPATH = [ 'build' ])
readsof.Program(
        target = 'build/readsof',
        source = find_files('source/readsof/','*.c'))
readsof.Depends('readsof', 'sof')

# SCLPL Compiler
src_files = find_files('source/compiler/','*.scm')
scheme.Program(
        target = 'build/slc',
        source = src_files)

# SCLPL Package Manager
src_files = find_files('source/slpkg/','*.scm')
scheme.Program(
        target = 'build/slpkg',
        source = src_files)

# Compiler Test Suite
#scheme.TestRunner(
#        target = 'build/tests/sclpl-cc-tests',
#        source = [s for s in src_files if not s.endswith("main.scm")] +
#                 find_files('tests/compiler/','*.scm'))

