-------------------------------------------------------------------------------
-- SCLPL Distribution Build Configuration
-------------------------------------------------------------------------------
solution "SCLPL Distribution"
configurations { "Release" }
targetdir "build"

-------------------------------------------------------------------------------
-- SCLPL Runtime
-------------------------------------------------------------------------------
project "sclpl-rt"
    kind "SharedLib"
    language "C"
    location "build"
    files { "source/runtime/**.*" }

project "sclpl-rt-tests"
    kind "ConsoleApp"
    language "C++"
    location "build"
    links { "UnitTest++", "sclpl-rt" }
    includedirs { "source/runtime/**", "tools/UnitTest++/**" }
    files { "tests/runtime/*.c*" }
    postbuildcommands { "./sclpl-rt-tests" }

-------------------------------------------------------------------------------
-- SCLPL Lexer
-------------------------------------------------------------------------------
project "sclpl-lex"
    kind "ConsoleApp"
    language "C"
    location "build"
    includedirs {
        "source/lexer/**",
        "source/common/",
        "source/runtime/**"
    }
    files {
        "source/lexer/**.*",
        "source/runtime/collector/**.*"
    }

project "sclpl-lex-tests"
    kind "ConsoleApp"
    language "C++"
    location "build"
    links { "UnitTest++" }
    includedirs { "source/lexer/**", "tools/UnitTest++/**" }
    files { "tests/lexer/*.c*" }
    postbuildcommands { "./sclpl-lex-tests" }

-------------------------------------------------------------------------------
-- SCLPL Parser
-------------------------------------------------------------------------------
project "sclpl-parse"
    kind "ConsoleApp"
    language "C"
    location "build"
    includedirs {
        "source/lexer/**",
        "source/runtime/**",
        "source/common/**"
    }
    files {
        "source/parser/**.*",
        "source/common/**.*",
        "source/runtime/collector/**.*"
    }

project "sclpl-parse-tests"
    kind "ConsoleApp"
    language "C++"
    location "build"
    links { "UnitTest++" }
    includedirs { "source/parser/**", "tools/UnitTest++/**" }
    files { "tests/parser/*.c*" }
    postbuildcommands { "./sclpl-parse-tests" }

-------------------------------------------------------------------------------
-- UnitTest++ - A C/C++ unit testing library
-------------------------------------------------------------------------------
project "UnitTest++"
    kind "SharedLib"
    language "C++"
    location "build"
    files {
        "tools/UnitTest++/src/*.*",
    }
    if os.is "windows" then
        files { "tools/UnitTest++/src/Win32/**.*" }
    else
        files { "tools/UnitTest++/src/Posix/**.*" }
    end

-------------------------------------------------------------------------------
-- Miscellaneous
-------------------------------------------------------------------------------
if _ACTION == "clean" then
    os.rmdir("build")
end

