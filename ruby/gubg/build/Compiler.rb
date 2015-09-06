module Build
    class Compiler
        def initialize()
            @include_paths = []
        end

        def add_include_path(path)
            @include_paths += [path].flatten
        end
    end
end
