require "rspec"
require "rspec/core/rake_task"
require './build-system/setup'

def windows?
  RbConfig::CONFIG['host_os'] =~ /mswin|msys|mingw|cygwin|bccwin|wince|emc/
end

#------------------------------------------------------------------------------
# Envrionment Definitions
#------------------------------------------------------------------------------
# Define the compiler environment
base_env = BuildEnv.new(echo: :command) do |env|
  env.build_dir('source','build/obj/source')
  env.build_dir('modules','build/obj/modules')
  env.set_toolset(:gcc)
  env["CFLAGS"] += ['-DLEAK_DETECT_LEVEL=1', '--std=c99', '-Wall', '-Wextra'] #, '-Werror']
  env["CPPPATH"] += ['modules/libopts/source'] + Dir['modules/libcds/source/**/']
end

main_env = base_env.clone do |env|
  env["CFLAGS"] += ['-O3']
end

test_env = base_env.clone do |env|
  env.build_dir('source','build/obj/test/source')
  env.build_dir('modules','build/obj/test/modules')
  env['CFLAGS'] +=  ['--coverage']
  env['LDFLAGS'] += ['--coverage']
end

#------------------------------------------------------------------------------
# Clang Toolchain Targets
#------------------------------------------------------------------------------
CLANG_BUILD_DIR = 'build/llvm'
CLANG_BIN_DIR = 'build/llvm/bin'
CLANG_BIN_NAME = 'clang'
CLANG_SRC_DIR = 'source/vendor/llvm-3.4.2'
CLANG_CMAKE_GENERATOR = ENV['CMAKE_GENERATOR'] || "Unix Makefiles"
CLANG_CMAKE_OPTS = [ '-DCMAKE_BUILD_TYPE=Release' ]
CLANG_MAKE_CMD = windows? ? 'nmake' : 'make'

file "#{CLANG_BUILD_DIR}/Makefile" => FileList["#{CLANG_SRC_DIR}/cmake/**/*"] do
    FileUtils.mkdir_p(CLANG_BUILD_DIR)
    FileUtils.cd(CLANG_BUILD_DIR) do
        sh "cmake #{CLANG_CMAKE_OPTS.join} -G\"#{CLANG_CMAKE_GENERATOR}\" ../../#{CLANG_SRC_DIR}"
    end
end

file "#{CLANG_BIN_DIR}/#{CLANG_BIN_NAME}" => ["#{CLANG_BUILD_DIR}/Makefile"] + FileList["#{CLANG_SRC_DIR}/tools/clang/**/*.c"] do
    FileUtils.cd(CLANG_BUILD_DIR) do
        sh "#{CLANG_MAKE_CMD} clang llvm-ar"
    end
end

task :clang => ["#{CLANG_BIN_DIR}/#{CLANG_BIN_NAME}"]

# Register clang with the environment
ENV['PATH'] = "#{CLANG_BIN_DIR}#{windows? ? ';':':'}#{ENV['PATH']}"

#------------------------------------------------------------------------------
# Rscons Build Targets
#------------------------------------------------------------------------------
task :default => [:build]

desc "Build all targets"
task :build => [:clang, :sclpl]

desc "Build the sclpl compiler and interpreter"
task :sclpl => [:libcds, :libopts] do
  main_env.Program('build/bin/sclpl',
    FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
end

task :libcds do
  main_env.Library('build/lib/libcds.a', FileList['modules/libcds/source/**/*.c'])
end

task :libopts do
  main_env.Library('build/lib/libopts.a', FileList['modules/libopts/source/**/*.c'])
end

#------------------------------------------------------------------------------
# Cleanup Target
#------------------------------------------------------------------------------
desc "Clean all rscons generated files and directories"
task(:clean) { Rscons.clean }

desc "Clean object files generated for vendor libraries as well as rscons"
task :clean_all => [:clean] do
  if Dir.exist? CLANG_BUILD_DIR
    FileUtils.cd(CLANG_BUILD_DIR) { sh "#{CLANG_MAKE_CMD} clean" }
  end
end

desc "Clobber the build directory and all it's contents"
task(:clobber) { FileUtils.rm_rf('build/') }

#------------------------------------------------------------------------------
# RSpec Tests for the Compiler
#------------------------------------------------------------------------------
# Make the specs depend on the libs
task :spec => [:libcds, :libopts]

# Build and test the compiler with coverage
RSpec::Core::RakeTask.new(:spec) do |t,args|
  t.rspec_opts = ['--format', 'documentation']
  test_env.Program('build/bin/sclpl-test',
    FileList['source/sclpl/*.c', 'build/lib/libopts.a', 'build/lib/libcds.a'])
  main_env.process
  test_env.process
end
