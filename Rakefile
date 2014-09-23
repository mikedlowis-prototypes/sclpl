require './build-system/setup'

# Define the compiler environment
BaseEnv = BuildEnv.new(echo: :command) do |env|
  env.build_dir('source','build/obj/source')
  env.set_toolset(:clang)
#  env['CC'] = 'clang'
#  env['CXX'] = 'clang'
#  env['LD'] = 'clang'
  env["CFLAGS"] += ['--std=gnu99', '-Wall', '-Wextra' ]#, '-Werror']
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
        sh "#{CLANG_MAKE_CMD} clang"
    end
end

task :clang => ["#{CLANG_BIN_DIR}/#{CLANG_BIN_NAME}"] do
    ENV['PATH'] = "#{CLANG_BIN_DIR}#{windows? ? ';':':'}#{ENV['PATH']}"
end

#------------------------------------------------------------------------------
# Rscons Build Targets
#------------------------------------------------------------------------------
task :default => [:build]

desc "Build all targets"
task :build => [:clang, :sclpl]

desc "Build the sclpl compiler and interpreter"
task :sclpl => ['source/sclpl/grammar.c'] do
  BaseEnv.Program('build/bin/sclpl', FileList['source/sclpl/*.c'])
end

file 'source/sclpl/grammar.c' => ['source/sclpl/grammar.y'] do
  grammar = File.readlines('source/sclpl/grammar.y').map{|l| l.chomp().inspect }
  File.open('source/sclpl/grammar.c','w') do |f|
    f.write("const char Grammar[] = \n");
    grammar.each { |l| f.write("#{l}\n") }
    f.write(";\n");
  end
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

desc "Clobber the build driectory and all it's contents"
task(:clobber) { FileUtils.rm_rf('build/') }

