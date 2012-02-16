require("gubg/utils")

class Tree
    attr_reader(:files)
    def initialize(root, reWantedFiles)
        @files = []
        Dir.each(root) do |dir, fn|
            @files << File.new(self, ::File.expand_path(fn, dir)) if reWantedFiles =~ fn
            :ok
        end
    end
    def find(str, strategy)
        case strategy
        when :exact
            @files.find{|file|file.name[1, file.name.length] == str[1, str.length]}
        when :approx
            @files.select{|file|file.name[-str.length, str.length] == str}
        else
            raise("Unknown search strategy #{strategy}")
        end
    end
    class File
        attr_reader(:tree, :name)
        def initialize(tree, filename)
            @tree, @name = tree, filename
        end
        def to_s
            @name
        end
    end
end
