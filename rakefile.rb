require("rake/clean")
require("benchmark")

task :default => :help

#help
task :help do
    puts("Help for gubg. These are the rake tasks that currently exist:")
    puts("clean: remove unwanted files")
    puts("fff: build the flexible file factory")
    puts("git_tools: install git helper scripts")
    puts("vim: install vim goodies")
    puts("poco: install the poco library")
end

def os()
    case RUBY_PLATFORM
    when /mingw/ then :windows
    when /linux/ then :linux
    else nil end
end
fail("Could not guess os from #{RUBY_PLATFORM}") if os.nil?

#clean
CLEAN.include(FileList["**/*.cpp_obj"])
CLEAN.include(FileList["**/*.obj"])
CLEAN.include(FileList["**/.*.obj"])
CLEAN.include(FileList["**/*.o"])
CLEAN.include(FileList["**/*.exe"])
CLEAN.include(FileList["**/*.dll"])
task :clean do
    Dir.chdir("cpp/apps/fff"){sh "rake clean"}
    rm_rf ENV['FFF_CACHE'] if ENV['FFF_CACHE']
end

gubg_dir = ENV["GUBG"] || Dir.getwd; ENV["GUBG"] = gubg_dir
build_dir = ENV["GUBG_BUILD"] || File.join(gubg_dir, 'GUBG_BUILD')
raise("You have to define GUBG_BUILD, the location where I can place my mojo") unless build_dir
directory build_dir
sdks_dir = ENV["GUBG_SDKS"] || File.join(build_dir, "sdks")
directory sdks_dir
bin_dir = ENV["GUBG_BIN"] || File.join(build_dir, "bin"); ENV["GUBG_BIN"] = bin_dir
ENV["PATH"] = ENV["PATH"] + ':' + bin_dir
directory bin_dir

task :submods do
    sh 'git submodule update --init'
end

#fff
task :fff do
    Dir.chdir("cpp/apps/fff") do
        sh "rake -j8 -m build"
        # sh "rake build"
        sh "rake self"
    end
end

#pa
task :pa do
    Dir.chdir("cpp/apps/pa") do
        sh "fff pa.cpp norun"
        cp("pa.exe", bin_dir)
    end
end

#hours
task :hours do
    Dir.chdir("cpp/apps/hours") do
        sh "fff hours.cpp norun"
        cp("hours.exe", bin_dir)
    end
end

#git_tools
task :git_tools => bin_dir do
    bash = "\#!"+`which bash`
    Dir.chdir(bin_dir) do
        [
            {name: 'qs', command: 'git status'},
            {name: 'qd', command: 'git diff'},
            {name: 'qc', command: 'git commit -a'},
            {name: 'qp', command: 'git pull --rebase'},
        ].each do |h|
            puts("Creating #{h[:name]}")
            File.open(h[:name], "w"){|fo|fo.puts(bash);fo.puts(h[:command])}
            File.chmod(0755, h[:name])
        end
    end
end

#poco: requires glibc-static to be installed on arch, maybe some tests link against librt statically.
#Installation of glibc-static failed on gpg problems, removed ~/.gnupg and getting the missing keys
#via "gpg --recv-keys <key>" twice solved the issue.
namespace :poco do
    dir = 'cpp/libs/extern/poco'
    task :update do
        if !File.exist?(File.join(dir, 'configure'))
            sh "git submodule --init update #{dir}"
        end
    end
    task :clean do
        Dir.chdir(dir) do
            sh 'make clean'
            rm_rf File.join(dir, 'lib')
        end
    end
    task :build do
        Dir.chdir(dir) do
            sh './configure --static --no-tests --omit=XML,JSON,Util,Net,Crypto,NetSSL_OpenSSL,Data,Data/SQLite,Data/ODBC,Data/MySQL,MongoDB,Zip,PageCompiler,PageCompiler/File2Page,CppParser,PDF'
            sh 'make -j8'
        end
    end
end
task :poco => %w[poco:update poco:build]

task :vim do
    gubg_dir = Dir.getwd
    Dir.chdir(ENV['HOME']) do
        symlink(File.join(gubg_dir, "vim/"), '.vim') unless File.exist?('.vim')
        symlink(File.join(gubg_dir, "vim/config.linux.vim"), '.vimrc') unless File.exist?('.vimrc')
    end
end
#YouCompleteMe vim plugin
task :ycm do
    Dir.chdir('vim/bundle/YouCompleteMe') do
        sh 'git submodule update --init --recursive'
        sh './install.sh'
    end
end

namespace :sfml do
    sfml_dir = "#{sdks_dir}/SFML"
    sfml_build_dir = "#{sdks_dir}/SFML/build"
    task :clean do
        rm_rf sfml_dir
    end
    file sfml_dir => sdks_dir do
        Dir.chdir(sdks_dir) do
            sh "git clone -b master https://github.com/LaurentGomila/SFML"
        end
    end
    task :build => sfml_dir do
        mkdir sfml_build_dir unless File.exist?(sfml_build_dir)
        Dir.chdir(sfml_build_dir) do
            generator = {windows: "-G \"Visual Studio 12 2013 Win64\"", linux: ""}[os]
            sh "cmake .. #{generator}"
            case os
            when :windows
                fail("Could not find solution") unless File.exist?("SFML.sln")
                sh "msbuild /p:Platform=x64 /p:Configuration=Release SFML.sln /t:Build"
            else
                sh "make"
            end
        end
    end
    task :install => sfml_build_dir do
        Dir.chdir(sfml_build_dir) do
            if File.exist?("build/SFML.sln")
                cp FileList["build/lib/Release/*.dll"], bin_dir
                cp FileList["build/lib/Release/*.lib"], lib_dir
                cp_r "include/SFML", inc_dir
            else
                sh "sudo make install"
            end
        end
    end
end

#testing
def find_tests(type)
    tests = FileList["cpp/libs/gubg/**/test/*_#{type}.cpp"]
    tests.exclude("cpp/libs/gubg/deprecated/**/*_#{type}.cpp")
    tests
end
def build_tests(type)
        report = {}
        total_duration = Benchmark.realtime do
            find_tests(type).each do |test|
                sh "fff #{test} norun" do |ok, res|
                    report[test] = {status: ok}
                end
            end
        end
        report.each do |test, info|
            puts("#{test} FAILED") unless info[:status]
        end
        puts("BUILD: #{report.values.select{|info|info[:status]}.length}/#{report.length} OK")
        puts("BUILD: #{total_duration} s")
end
def run_tests(type)
        report = {}
        total_duration = Benchmark.realtime do
            find_tests(type).each do |test|
                duration = Benchmark.realtime do
                    sh "fff #{test}" do |ok, res|
                        report[test] = {status: ok}
                    end
                end
                report[test][:duration] = duration
            end
        end
        report.each do |test, info|
            if not info[:status]
                puts("#{test} FAILED")
            elsif info[:duration] > 1.0
                puts("#{test} SLOW #{info[:duration]}") 
            end
        end
        puts("RUN: #{report.values.select{|info|info[:status]}.length}/#{report.length} OK")
        puts("RUN: #{total_duration} s")
end
#unit tests
namespace :uts do
    task :build do
        build_tests("tests")
    end
    task :run => :build do
        run_tests("tests")
    end
end
#manual tests
namespace :mts do
    task :build do
        build_tests("mtests")
    end
    task :run => :build do
        run_tests("mtests")
    end
end
#scenario tests
namespace :sts do
    task :build do
        build_tests("stests")
    end
    task :run => :build do
        run_tests("stests")
    end
end
#performance tests
namespace :pts do
    task :build do
        build_tests("ptests")
    end
    task :run => :build do
        run_tests("ptests")
    end
end
#all tests
namespace :ats do
    task :build => %w[uts:build sts:build pts:build]
    task :run => %w[uts:run sts:run pts:run]
end
