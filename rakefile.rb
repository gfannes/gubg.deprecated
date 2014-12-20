require("rake/clean")
require("benchmark")

task :default => :help

#help
task :help do
    puts("Help for gubg. These are the rake tasks that currently exist:")
    puts("clean: remove unwanted files")
    puts("fff: build the flexible file factory")
end

def os()
    case RUBY_PLATFORM
    when /mingw/ then :win
    when /linux/ then :linux
    else nil end
end
fail("Could not guess os from #{RUBY_PLATFORM}") if os.nil?

#clean
CLEAN.include(FileList["**/*.cpp_obj"])
CLEAN.include(FileList["**/*.obj"])
CLEAN.include(FileList["**/*.o"])
CLEAN.include(FileList["**/*.exe"])
CLEAN.include(FileList["**/*.dll"])

#fff
task :fff do
    Dir.chdir("cpp/apps/fff") do
        sh "rake clean fff self"
    end
end

sdks_dir = ENV["GUBG_SDKS"]
bin_dir = ENV["GUBG_BIN"]
inc_dir = ENV["GUBG_INC"]
lib_dir = ENV["GUBG_LIB"]

directory sdks_dir
namespace :sfml do
    sfml_dir = "#{sdks_dir}/SFML"
    task :clean do
        rm_rf sfml_dir
    end
    file sfml_dir => sdks_dir do
        Dir.chdir(sdks_dir) do
            sh "git clone -b master https://github.com/gfannes/SFML"
        end
    end
    task :build => sfml_dir do
        Dir.chdir(sfml_dir) do
            mkdir "build" unless File.exist?("build")
            Dir.chdir("build") do
                generator = {win: "-G \"Visual Studio 12 2013 Win64\"", linux: ""}[os]
                sh "cmake .. #{generator}"
                case os
                when :win
                    fail("Could not find solution") unless File.exist?("SFML.sln")
                    sh "msbuild /p:Platform=x64 /p:Configuration=Release SFML.sln /t:Build"
                else
                    sh "make"
                end
            end
        end
    end
    task :install => sfml_dir do
        Dir.chdir(sfml_dir) do
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
