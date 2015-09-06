require(File.join(ENV['GUBG'], 'ruby/gubg/build/Executable.rb'))
exe = Build::Executable.new('test.exe')
exe.add_sources('test.cpp')

task :test do
    exe.execute
end
