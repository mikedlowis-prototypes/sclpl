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

# Scheme program builder
scheme_builder = Builder(
    action = 'csc -o $TARGET $SOURCE',
    suffix = '.exe' if (platform.system() == 'Windows') else ''
)

# Create the Environment for this project
env = Environment(
    ENV = os.environ,
    BUILDERS = { 'SchemeProgram': scheme_builder }
)

#------------------------------------------------------------------------------
# SCLPL Targets
#------------------------------------------------------------------------------

# SCLPL Compiler
env.SchemeProgram(
    target = 'sclpl-cc.exe',
    source = find_files('source/compiler/','*.scm')
)

