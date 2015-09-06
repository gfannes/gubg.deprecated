require('gubg/build/Compiler.rb')

module Build
    class GCC < Compiler
        def compile_command(object, source)
            include_paths_cmd = @include_paths.map{|ip|"-I#{ip}"}*' '
            "g++ -std=c++11 -c #{source} -o #{object} #{include_paths_cmd}"
        end
        def link_command(exe, objects)
            "g++ -o #{exe} #{objects*' '}"
        end
    end
end
