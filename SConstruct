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

# Scheme Source Compiler
scheme_compiler = Builder(
    action        = 'csc $CCFLAGS -c -o $TARGET $SOURCE',
    suffix        = '.o',
    src_suffix    = '.scm',
    single_source = True
)

# Scheme Binary Linker
scheme_linker = Builder(
    action      = 'csc $LDFLAGS -o $TARGET $SOURCES',
    suffix      = "$PROGSUFFIX",
    src_suffix  = '.o',
    src_builder = [ scheme_compiler ]
)

# Create the Environment for this project
env = Environment(
        ENV      = os.environ,
        CCFLAGS  = [ '-explicit-use' ],
        LDFLAGS  = [],
        BUILDERS = { 'SchemeProgram': scheme_linker }
)

#------------------------------------------------------------------------------
# SCLPL Targets
#------------------------------------------------------------------------------

# SCLPL Compiler
env.SchemeProgram(
    target = 'sclpl-cc',
    source = find_files('source/compiler/','*.scm')
)

