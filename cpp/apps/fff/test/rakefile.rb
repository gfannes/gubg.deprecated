task :run do
    sh "fff Linker_tests.cpp c++.tree:.."
    sh "fff Compiler_tests.cpp c++.tree:.."
end
