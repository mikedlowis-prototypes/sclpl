#------------------------------------------------------------------------------
# Environment Setup and Utilities
#------------------------------------------------------------------------------

import fnmatch
import os
import platform

# Helper function for recursively finding files
def find_files(dir,pattern):
    matches = []
    for root, dirnames, filenames in os.walk(dir):
        for filename in fnmatch.filter(filenames, pattern):
            matches.append(os.path.join(root, filename))
    return matches

# Scheme Source Compiler
scheme_compiler = Builder(
    action     = 'csc -c -o $TARGET $SOURCE',
    suffix     = '.o',
    src_suffix = '.scm',
    single_source = True
)

# Scheme Binary Linker
scheme_linker = Builder(
    action  = 'csc -o $TARGET $SOURCE',
    suffix = "$PROGSUFFIX",
    src_builder = [ scheme_compiler ]
)

# Create the Environment for this project
env = Environment(
        ENV = os.environ,
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

