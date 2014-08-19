#------------------------------------------------------------------------------
# Environment Setup and Utilities
#------------------------------------------------------------------------------
import platform
import fnmatch
import os
import glob

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

# Base Environment
#-----------------
base = Environment(
    ENV     = os.environ,
    CCFLAGS = [],
    LDFLAGS = [],
    LIBPATH = ['build/lib'])

# On windows use mingw because VS is not C99 compliant
base_cpp = base.Clone()
if platform.system() == 'Windows':
    base_cpp.Append(TOOLS=['mingw'])

# Default C/C++ Environment
#---------------------------
c_cpp = base_cpp.Clone(CCFLAGS = [ '-Wall', '-Werror', '-std=c99' ])

# C/C++ Environment Minus Standard Lib
#-------------------------------------
nostdlib = c_cpp.Clone(LINKFLAGS = [ '-nostdlib' ],
                       LIBS      = ['gcc'])

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
    suffix      = '$PROGSUFFIX',
    src_suffix  = '.o',
    src_builder = [ scheme_compiler ])

# Create the Environment for this project
scheme = base.Clone(CCFLAGS  = [ '-I', 'inc'],
                    BUILDERS = { 'Program': scheme_linker })

#------------------------------------------------------------------------------
# Vendor Targets
#------------------------------------------------------------------------------
#import os
#
#llvm = base.Clone()
#if platform.system() == 'Windows':
#    llvm.Append(CMAKE_GENERATOR='NMake Makefiles',
#                MAKECMD='cd build/llvm/ && nmake')
#else:
#    llvm.Append(CMAKE_GENERATOR='Unix Makefiles',
#                MAKECMD='make -C build/llvm/')
#
#llvm.Command('build/llvm/Makefile',
#             'source/vendor/llvm-3.4.2/',
#             'cd ${TARGET.dir} && cmake -G"${CMAKE_GENERATOR}" ../../${SOURCE}')
#llvm.Command('build/llvm/Release+Assert/bin/llc',
#             'build/llvm/Makefile',
#             '${MAKECMD}')

#------------------------------------------------------------------------------
# SCLPL Targets
#------------------------------------------------------------------------------

# SOF Shared Library
c_cpp.SharedLibrary('build/lib/sof', find_files('source/libsof/','*.c'))

# SBC Shared Library
#c_cpp.SharedLibrary('build/lib/sbc', find_files('source/libsbc/','*.c'))

# readsof Command Line Utility
#readsof = c_cpp.Clone(CPPPATH = [ 'source/libsof/' ],
#                      LIBS    = [ 'sof' ],
#                      LIBPATH = [ 'build' ])
#readsof.Program('build/readsof', find_files('source/readsof/','*.c'))
#readsof.Depends('readsof', 'sof')

## Register executable targets
#for name in ['slc', 'slpkg', 'slas']:
#    scheme.Program('bin/'+name, find_files('source/'+name, '*.scm'))
#
## SCLPL Virtual Machine
##----------------------
## Virtual Machine Kernel
#nostdlib.StaticLibrary('build/lib/vmkernel', glob.glob('source/slvm/kernel/*.c'))
#
## Standard Platform Library (C99)
#c_cpp.StaticLibrary('build/lib/stdpf',
#                    glob.glob('source/slvm/platforms/C99/*.c'),
#                    CPPPATH = ['source/slvm/kernel'])
#
## VM Executable Using Standard Platform
#c_cpp.Program('build/bin/slvm', [], LIBS = ['vmkernel', 'stdpf'])
#
## Build all VM Extensions
#for ext in glob.glob('source/slvm/extensions/*/'):
#    name = os.path.basename(ext.strip('\\/'))
#    c_cpp.StaticLibrary('build/lib/'+name+'ext',
#                        glob.glob(ext + '/*.c'),
#                        CPPPATH = ['source/slvm/kernel'],
#                        LIBS = ['stdpf'])
#
