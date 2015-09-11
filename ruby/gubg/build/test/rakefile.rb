require(File.join(ENV['GUBG'], 'ruby/gubg/build/Executable.rb'))
prog = Build::Executable.new('test.exe')
prog.add_sources(FileList.new('src/*'))

task :clean => prog.clean_rule
task :build => prog.build_rule
task :test => prog.build_rule do
    sh "./#{prog.exe_filename}"
end
