require("rake/clean")

task :default => :help

#help
task :help do
    puts("Help for gubg. These are the rake tasks that currently exist:")
    puts("clean: remove unwanted files")
    puts("fff: build the flexible file factory")
end

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

#run unit tests
task :uts do
    tests = FileList["cpp/libs/gubg/**/test/*_tests.cpp"]
    tests.exclude("cpp/libs/gubg/file/test/Descriptor_tests.cpp")
    tests.exclude("cpp/libs/gubg/internet/test/Client_tests.cpp")
    tests.exclude("cpp/libs/gubg/internet/test/Server_tests.cpp")
    tests.exclude("cpp/libs/gubg/tcp/test/Socket_tests.cpp")
    report = {}
    tests.each do |test|
        sh "fff #{test}" do |ok, res|
            report[test] = ok
        end
    end
    report.each do |test, ok|
        puts("#{test} FAILED") unless ok
    end
    puts("#{report.values.select{|ok|ok}.length}/#{report.length} OK")
end
