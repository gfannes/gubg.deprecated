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
    def find(filename)
        @files.find{|file|file.name == filename}
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
