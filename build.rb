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
  # Compiler options
  env["CFLAGS"] += ['-DLEAK_DETECT_LEVEL=1', '--std=c99', '-Wall', '-Wextra']#, '-Werror']
  env["CPPPATH"] += Dir['modules/libcds/source/**/'] + [
    'modules/libopts/source',
    'source/',
  ]
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
# Release Build Targets
#------------------------------------------------------------------------------
# Build the compiler
sources = FileList['source/*.c']
main_env.Program('build/bin/sclpl', sources)

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
if Opts[:profile].include? "test"
  test_env.Program('build/bin/sclpl-test', sources)
  test_env.Command('RSpec', [], 'CMD' => [
      'rspec', '--pattern', 'spec/**{,/*/**}/*_spec.rb', '--format', 'documentation'])
end
