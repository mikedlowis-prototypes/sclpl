#!/usr/bin/env ruby
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

  # Add a builder for creating a CMake project
  env.add_builder :CMake do |target, sources, cache, env, vars|
    target_dir = File.dirname(target)
    source_dir = "../" * target_dir.split(/\\|\//).length + File.dirname(sources.first)
    cmd = env.expand_varref("${CMAKE_CMD}", vars.merge('_SOURCES' => source_dir))
    unless cache.up_to_date?(target, cmd, sources, env)
      cache.mkdir_p(target_dir)
      Dir.chdir(target_dir) { env.execute("CMake #{target}", cmd) }
      cache.register_build(target, cmd, sources, env)
    end
    target if File.exists? target
  end

  # Add a builder for building a project with Make
  env.add_builder :Make do |target, sources, cache, env, vars|
    working_dir = File.dirname(sources.first)
    cmd = env.expand_varref("${MAKE_CMD}", vars)
    unless cache.up_to_date?(target, cmd, sources, env)
      Dir.chdir(working_dir) { env.execute("Make #{target}", cmd) }
      cache.register_build(target, cmd, sources, env)
    end
    target if File.exists? target
  end

  # CMake Configuration
  env['CMAKE_GENERATOR'] = ENV['CMAKE_GENERATOR'] || "#{windows? ? 'NMake' : 'Unix'} Makefiles"
  env['CMAKE_FLAGS']     = []
  env['CMAKE_CMD']       = ['cmake', '-G', '${CMAKE_GENERATOR}', '${CMAKE_FLAGS}', '${_SOURCES}']
  env['MAKE_CMD']        = [windows? ? 'nmake' : 'make']

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
  env['CFLAGS'] +=  ['-O0']
  if Opts[:profile].include? "coverage"
    env['CFLAGS']  << '--coverage'
    env['LDFLAGS'] << '--coverage'
  end
end

#------------------------------------------------------------------------------
# Clang Toolchain Targets
#------------------------------------------------------------------------------
#main_env.CMake('build/llvm/Makefile',
#               Dir['source/vendor/llvm-3.4.2/CMakeLists.txt',
#                   'source/vendor/llvm-3.4.2/cmake/**/*/'])
#main_env.Make("build/llvm/bin/clang#{windows? ? '.exe':''}",
#              ['build/llvm/Makefile'] + Dir['source/vendor/llvm-3.4.2/tools/**/*.*'])

# Register clang with the environment
#ENV['PATH'] = "build/llvm/bin/#{windows? ? ';':':'}#{ENV['PATH']}"

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
if Opts[:profile].include? "test"
  test_env.Program('build/bin/sclpl-test',
    FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
  test_env.Command('RSpec', [], 'CMD' => [
      'rspec', '--pattern', 'spec/**{,/*/**}/*_spec.rb', '--format', 'documentation'])
end
