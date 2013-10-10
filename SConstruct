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
        TOOLS    = [ 'mingw' ])

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
            'Program':    scheme_linker,
            'TestRunner': scheme_tester })

#------------------------------------------------------------------------------
# SCLPL Targets
#------------------------------------------------------------------------------

# SOF Shared Library
c_cpp.SharedLibrary('build/sof', find_files('source/libsof/','*.c'))

# SBC Shared Library
c_cpp.SharedLibrary('build/sbc', find_files('source/libsbc/','*.c'))

# readsof Command Line Utility
readsof = c_cpp.Clone(
        CPPPATH = [ 'source/libsof/' ],
        LIBS = [ 'sof' ],
        LIBPATH = [ 'build' ])
readsof.Program('build/readsof', find_files('source/readsof/','*.c'))
readsof.Depends('readsof', 'sof')

# SCLPL Compiler
scheme.Program('build/slc', find_files('source/compiler/','*.scm'))

# SCLPL Package Manager
scheme.Program('build/slpkg', find_files('source/slpkg/','*.scm'))

# SCLPL Assembler
c_cpp.Program('build/slas', find_files('source/slas/','*.c'))

# SCLPL Virtual Machine
c_cpp.Program('build/slvm', find_files('source/slvm/','*.c'))

