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

# Helper function to build scheme programs and test runners
def SchemeBuildAndTest(target,sources,tests):
    # Figure out the target names
    #test_runner  = target + '_tests'
    #test_output  = target + '_results'
    #test_sources = [e for e in sources if not e.endswith('main.scm')] + tests
    # Create the targets
    scheme.Program( target, sources )
    #scheme.Program( test_runner, test_sources )
    #RunTest( test_output, test_runner )

# Helper function to run a test suite and generate a log file
def RunTest( output, runner ):
    runner_exe = runner + ('.exe' if (platform.system() == 'Windows') else '')
    cmd = os.path.basename(runner_exe)
    scheme.Command( output, runner_exe, cmd + ' > $TARGET')

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

# Create the Environment for this project
scheme = Environment(
        ENV      = os.environ,
        CCFLAGS  = [ '-I', 'inc'],
        LDFLAGS  = [],
        TOOLS    = [ 'mingw' ],
        BUILDERS = { 'Program': scheme_linker })
scheme.PrependENVPath('PATH', './build')

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
SchemeBuildAndTest( 'build/slc',
                    find_files('source/slc/','*.scm'),
                    find_files('tests/slc/','*.scm') )

# SCLPL Package Manager
SchemeBuildAndTest( 'build/slpkg',
                    find_files('source/slpkg/','*.scm'),
                    find_files('tests/slpkg/','*.scm') )

# SCLPL Assembler
SchemeBuildAndTest( 'build/slas',
                    find_files('source/slas/','*.scm'),
                    find_files('tests/slas/','*.scm') )

# SCLPL Virtual Machine
c_cpp.Program('build/slvm', find_files('source/slvm/','*.c'))

