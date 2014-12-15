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

def obj_fn(src_fn)
    src_fn.gsub(/\.(c|cpp)$/, ".obj")
end
def cpp_fn(obj_fn)
    obj_fn.gsub(/\.obj$/, ".cpp")
end
def c_fn(obj_fn)
    obj_fn.gsub(/\.obj$/, ".c")
end

fff_cpp_fns = %w[fff Tag Value Board Create Execute ToolFactory Compiler Linker tools/Starter tools/ParseIncludes tools/ResolveHeader tools/Compiler tools/Linker tools/Runner tools/Search].map{|str|"#{str}.cpp"}
gubg_cpp_fns = %w[OptionParser Platform Strange file/File file/Filesystem file/Forest env/Util env/Variables hash/MD5 parse/cpp/pp/Lexer parse/cpp/pp/Token parse/Line chrono/Uptime lua/State].map{|str|"../../libs/gubg/#{str}.cpp"}
lua_c_fns = %w[lapi lauxlib lbaselib lbitlib lcode lcorolib lctype ldblib ldebug ldo ldump lfunc lgc linit liolib llex lmathlib lmem loadlib lobject lopcodes loslib lparser lstate lstring lstrlib ltable ltablib ltm lundump lvm lzio].map{|str|"../../../c/lua-5.2.3/#{str}.c"}
cpp_objects = (fff_cpp_fns+gubg_cpp_fns).map{|fn|obj_fn(fn)}
c_objects = (lua_c_fns).map{|fn|obj_fn(fn)}
objects = cpp_objects+c_objects
#Keep this the same name as created by fff to ensure the self-test is ok
fff_exe_fn = "fff.exe"
install_dir = ENV["GUBG_BIN"]
cache_dir = File.expand_path("fff", ENV["GUBG_TMP"])

task :fff do
    Rake::Task[:build].invoke
    Rake::Task[:install].invoke
end

directory install_dir
directory cache_dir

CLEAN.include(FileList["*.obj"])
CLEAN.include(FileList["*.exe"])
CLEAN.include(objects)
task :clean do
    rm_rf cache_dir
    Rake::Task[cache_dir].invoke
end

task :build => [install_dir, cache_dir] do
    Rake::Task[fff_exe_fn].invoke
end
rule ".obj" => ".cpp" do |task|
    source_fn = cpp_fn(task.name)
    include_paths = "-I.. -I../../libs -I../../../c/lua-5.2.3"
    options = []
    options << {debug: "-g", release: "-O3"}[config]
    defines = []
    defines += {debug: %w[DEBUG GUBG_DEBUG], release: %w[NDEBUG GUBG_RELEASE]}[config]
    sh "g++ -std=c++0x -c -o #{task.name} #{source_fn} #{options*" "} #{include_paths} #{defines.map{|d|"-D#{d}"}*" "}"
end
rule ".obj" => ".c" do |task|
    source_fn = c_fn(task.name)
    include_paths = ""
    options = "-O3"
    sh "gcc -c -o #{task.name} #{source_fn} #{options} #{include_paths}"
end
file fff_exe_fn => objects do
    flags = {windows: "", linux: "-pthread"}[os]
    sh "g++ -std=c++0x -o #{fff_exe_fn} #{objects*" "} #{flags}"

end

task :install => fff_exe_fn do
    cp fff_exe_fn, File.expand_path({windows: "fff.exe", linux: "fff"}[os], install_dir)
end

task :self do
    rm_f fff_exe_fn
    cnf = {debug: "debug", release: "release"}[config]
    sh "fff fff.cpp norun #{cnf}"
    fail("fff did not create #{fff_exe_fn}") unless File.exist?(fff_exe_fn)
    Rake::Task[:install].invoke
end
