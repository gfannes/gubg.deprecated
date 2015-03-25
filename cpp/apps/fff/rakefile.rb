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

fff_cpp_fns = %w[fff Tag Value Board Create Execute ToolFactory Compiler Linker tools/Starter tools/ParseIncludes tools/ResolveHeader tools/Compiler tools/Linker tools/Runner tools/Search].map{|str|"#{str}.cpp"}
gubg_cpp_fns = %w[OptionParser Platform Strange file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line chrono/Uptime].map{|str|"../../libs/gubg/#{str}.cpp"}
source_fns = fff_cpp_fns+gubg_cpp_fns

object_fns = []
cache_dir = File.join(Dir.getwd, '.cache')
directory cache_dir
source_fns.each do |source_fn|
    object_fn = File.join(cache_dir, source_fn.gsub(/[\.\/\\:]/, '_')+'.obj')
    include_paths = "-I.. -I../../libs"
    options = []
    options << {debug: "-g", release: "-O3"}[config]
    defines = []
    defines += {debug: %w[DEBUG GUBG_DEBUG], release: %w[NDEBUG GUBG_RELEASE]}[config]
    rule object_fn => cache_dir do
        sh "g++ -std=c++0x -c -o #{object_fn} #{source_fn} #{options*" "} #{include_paths} #{defines.map{|d|"-D#{d}"}*" "}"
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
    flags = {windows: "", linux: "-pthread"}[os]
    sh "g++ -std=c++0x -o #{fff_exe_fn} #{object_fns*" "} #{flags}"

end

installed_fff_exe = File.join(install_dir, {windows: "fff.exe", linux: "fff"}[os])
file installed_fff_exe => fff_exe_fn do
    cp fff_exe_fn, installed_fff_exe
end
task :install => installed_fff_exe

task :self do
    rm_f fff_exe_fn
    cnf = {debug: "debug", release: "release"}[config]
    sh "fff fff.cpp norun #{cnf}"
    fail("fff did not create #{fff_exe_fn}") unless File.exist?(fff_exe_fn)
    Rake::Task[:install].invoke
end
