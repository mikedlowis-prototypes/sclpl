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

  env.add_builder :Install do |target, sources, cache, env, vars|
    is_dir = (sources.length > 1) || Dir.exists?(sources.first)
    outdir = is_dir ? target : File.dirname(target)
    # Collect the list of files to copy over
    file_map = {}
    if is_dir
      sources.each do |src|
        if Dir.exists? src
          Dir["#{src}/**/*"].each do |subfile|
              subpath = Pathname.new(subfile).relative_path_from(Pathname.new(src)).to_s
              file_map[subfile] = "#{outdir}/#{subpath}"
          end
        else
          file_map[src] = "#{outdir}/#{File.basename(src)}"
        end
      end
    else
      file_map[sources.first] = target
    end
    # Check the cache and  copy if necessary
    unless cache.up_to_date?(target, :Install, file_map.keys, env)
      puts "INSTALL #{target}"
      file_map.each do |k,v|
        cache.mkdir_p(File.dirname(v))
        FileUtils.cp(k,v)
      end
      cache.register_build(target, :Install, file_map.keys, env)
    end
    target if (is_dir ? Dir.exists?(target) : File.exists?(target))
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
# Release Build Targets
#------------------------------------------------------------------------------
main_env.Library('build/lib/libcds.a', FileList['modules/libcds/source/**/*.c'])
main_env.Library('build/lib/libopts.a', FileList['modules/libopts/source/**/*.c'])
main_env.Program('build/bin/sclpl',
  FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
main_env.Install('build/include/sclpl.h', 'source/runtime/sclpl.h')

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
if Opts[:profile].include? "test"
  test_env.Program('build/bin/sclpl-test',
    FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
  test_env.Command('RSpec', [], 'CMD' => [
      'rspec', '--pattern', 'spec/**{,/*/**}/*_spec.rb', '--format', 'documentation'])
end
