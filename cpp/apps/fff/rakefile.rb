require("rake/clean")

os = case RUBY_PLATFORM
     when "x64-mingw32" then :windows
     when "x86_64-linux" then :linux
     else nil
     end
fail("Could not determine OS") unless os

config = case ENV["config"]
         when "debug", "DEBUG" then :debug
         else :release
         end

fff_cpp_fns = %w[fff Tag Value Board Create Execute AgentFactory Compiler Linker].map{|str|"#{str}.cpp"}
agents_cpp_fns = %w[Starter ExtractIncludes ResolveHeader Compiler Linker Runner Search Chai].map{|str|"agents/#{str}.cpp"}
gubg_cpp_fns = %w[OptionParser Platform Strange file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line chrono/Uptime chai/Engine].map{|str|"../../libs/gubg/#{str}.cpp"}
source_fns = fff_cpp_fns+agents_cpp_fns+gubg_cpp_fns

object_fns = []
cache_dir = File.join(Dir.getwd, '.cache')
directory cache_dir
source_fns.each do |source_fn|
    object_fn = File.join(cache_dir, source_fn.gsub(/[\.\/\\:]/, '_')+'.obj')
    include_paths = %w[.. ../../libs ../../libs/extern/ChaiScript/include ../../libs/extern/poco/Foundation/include]
    include_paths_str = include_paths.map{|p|"-I#{p}"}*' '
    options = []
    case os
    when :linux then options << {debug: "-g", release: "-O3"}[config]
    when :windows
        options << {debug: "/Zi", release: "/O2"}[config]
    else raise("Unknown os #{os}") end
    defines = []
    defines += {debug: %w[DEBUG GUBG_DEBUG], release: %w[NDEBUG GUBG_RELEASE]}[config]
    defines_str = defines.map{|d|"-D#{d}"}*' '
    rule object_fn => [cache_dir, source_fn] do
        case os
        when :linux then sh "g++ -std=c++0x -c -o #{object_fn} #{source_fn} #{options*' '} #{include_paths_str} #{defines_str}"
        when :windows then sh "cl /nologo -c /bigobj /EHsc /Fo#{object_fn} #{source_fn} #{options*" "} #{include_paths_str} #{defines_str}"
        else raise("Unknown os #{os}") end
    end
    object_fns << object_fn
end

#Keep this the same name as created by fff to ensure the self-test is ok
fff_exe_fn = "fff.exe"
install_dir = ENV["GUBG_BIN"]

directory install_dir

CLEAN.include(FileList["*.obj"])
CLEAN.include(FileList["*.exe"])
task :clean do
    rm_rf cache_dir
    Rake::Task[cache_dir].invoke
end

task :build => [install_dir, cache_dir] do
    Rake::Task[fff_exe_fn].invoke
end
file fff_exe_fn => object_fns do
    flags, libs_str, lib_paths_str = '', '', ''
    case os
    when :linux
        flags = '-pthread'
        libs = %w[dl PocoFoundation]
        libs_str = libs.map{|lib|"-l#{lib}"}*' '
        lib_paths = %w[../../libs/extern/poco/lib/Linux/x86_64]
        lib_paths_str = lib_paths.map{|p|"-L#{p}"}*' '
    when :windows
    else raise("Unknown os #{os}") end
    case os
    when :linux then sh "g++ -std=c++0x -o #{fff_exe_fn} #{object_fns*" "} #{flags} #{lib_paths_str} #{libs_str}"
    when :windows then sh "link /nologo /DEBUG /OUT:#{fff_exe_fn} #{object_fns*" "} #{flags} #{lib_paths_str} #{libs_str}"
    else raise("Unknown os #{os}") end
end

installed_fff_exe = File.join(install_dir, {windows: "fff.exe", linux: "fff"}[os])
file installed_fff_exe => fff_exe_fn do
    cp fff_exe_fn, installed_fff_exe
end
task :install => installed_fff_exe

task :self => fff_exe_fn do
	tmp_fff_exe = 'tmp_fff.exe'
	rm_f tmp_fff_exe
	mv fff_exe_fn, tmp_fff_exe
    rm_f fff_exe_fn
    cnf = {debug: "debug", release: "release"}[config]
    case os
    when :linux then sh('./'+tmp_fff_exe, 'fff.cpp', 'norun', cnf)
    when :windows then sh(tmp_fff_exe, 'fff.cpp', 'norun', cnf)
    else raise("Unknown os #{os}") end
    rm tmp_fff_exe
    fail("fff did not create #{fff_exe_fn}") unless File.exist?(fff_exe_fn)
    Rake::Task[:install].invoke
end
