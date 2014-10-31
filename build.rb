#!/bin/env ruby
require './build-system/setup'

def windows?
  RbConfig::CONFIG['host_os'] =~ /mswin|msys|mingw|cygwin|bccwin|wince|emc/
end

#------------------------------------------------------------------------------
# Environment Definitions
#------------------------------------------------------------------------------
# Define the default compiler environment
base_env = BuildEnv.new do |env|
  # Move the object files to the build dir
  env.build_dir('source','build/obj/source')
  env.build_dir('modules','build/obj/modules')

  # Use gcc toolchain
  env.set_toolset(:gcc)

  # Add a builder for creating directories
  env.add_builder :MakeDir do |target, sources, cache, env, vars|
    cache.mkdir_p(target)
    target if Dir.exists?(target)
  end

  # CMake Configuration
  env['CMAKE_GENERATOR'] = ENV['CMAKE_GENERATOR'] || "#{windows? ? 'NMake' : 'Unix'} Makefiles"
  env['CMAKE_FLAGS']     = []
  env['CMAKE_CMD']       = ['cmake', '-G', '${CMAKE_GENERATOR}', '${CMAKE_FLAGS}']
  env['MAKE_CMD']        = windows? ? 'nmake' : 'make'

  # Compiler options
  env["CFLAGS"] += ['-DLEAK_DETECT_LEVEL=1', '--std=c99', '-Wall', '-Wextra']#, '-Werror']
  env["CPPPATH"] += ['modules/libopts/source'] + Dir['modules/libcds/source/**/']
end

# Define the release environment
main_env = base_env.clone do |env|
  # Move the object files to the build dir
  env.build_dir('source','build/obj/source')
  env.build_dir('modules','build/obj/modules')
  env["CFLAGS"] += ['-O3']
end

# Define the test environment
test_env = base_env.clone do |env|
  # Move the object files to the build dir
  env.build_dir('source','build/obj_test/source')
  env.build_dir('modules','build/obj_test/modules')
  env['CFLAGS'] +=  ['-O0', '--coverage']
  env['LDFLAGS'] += ['--coverage']
end

#------------------------------------------------------------------------------
# Clang Toolchain Targets
#------------------------------------------------------------------------------
main_env.MakeDir('build/llvm', [])
main_env.Command('build/llvm/Makefile',
                 Dir['source/vendor/llvm-3.4.2/CMakeLists.txt',
                     'source/vendor/llvm-3.4.2/cmake/**/*/'],
                 'CMD' => ['cd', 'build/llvm/', '&&', '${CMAKE_CMD}', '../../source/vendor/llvm-3.4.2/'])
main_env.Command("build/llvm/bin/clang#{windows? ? '.exe':''}",
                 Dir['source/vendor/llvm-3.4.2/tools/**/*.*'],
                 'CMD' => ['cd', 'build/llvm', '&&', '${MAKE_CMD}'])

# Register clang with the environment
ENV['PATH'] = "build/llvm/bin/#{windows? ? ';':':'}#{ENV['PATH']}"

#------------------------------------------------------------------------------
# Release Build Targets
#------------------------------------------------------------------------------
main_env.Library('build/lib/libcds.a', FileList['modules/libcds/source/**/*.c'])
main_env.Library('build/lib/libopts.a', FileList['modules/libopts/source/**/*.c'])
main_env.Program('build/bin/sclpl',
  FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
test_env.Program('build/bin/sclpl-test',
  FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
test_env.Command('RSpec', [], 'CMD' => [
    'rspec', '--pattern', 'spec/**{,/*/**}/*_spec.rb', '--format', 'documentation'])

