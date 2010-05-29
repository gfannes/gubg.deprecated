#A Tree is a collection of files defined by a specific anchorPoint in a file system:
# * Step from anchorPoint towards "/" until "root.tree" is found
# * All files in all subdirectories from "root.tree" are part of the collection
# * "root.tree: can include other files collections or exclude other files

require("root")
require("gubg/utils")

class Tree
    @@rootFileName = "root.tree"

    def initialize(anchorPoint)
        @anchorPoint = ::File.expand_path(anchorPoint)
        loadRoot_
        createLocalFileCollection_
        loadChildren_
    end

    def each(&block)
        @filesPerExtension.each do |extension, files|
            files.each{|file|block.call(file)}
        end
    end

    private
    def loadRoot_
        #Find @rootFilePath: step down from @anchorPoint towards "/", until "root.tree" is found
        @rootFilePath = nil
        @rootDirectory = nil
        directory = ::File.directory?(@anchorPoint) ? @anchorPoint : ::File.dirname(@anchorPoint)
        loop do
            tmpRootFile = ::File.expand_path(@@rootFileName, directory)
            if ::File.exist?(tmpRootFile)
                @rootFilePath = tmpRootFile
                @rootDirectory = directory
                break
            end
            nextDirectory = ::File.dirname
            raise("No #{@@rootFile} found") if nextDirectory == directory
        end
        #Load "root.tree"
        puts("rootFilePath = #{@rootFilePath}")
        @root = Root.createFromFile(@rootFilePath)
    end
    class File
        attr(:directory, true)
        attr(:filename, true)
        def initialize(directory, fileName)
            @directory, @fileName = directory, fileName
            @extension = ::File.extname(@fileName)
        end
        def extension
            @extension
        end
        def to_s
            ::File.expand_path(@fileName, @directory)
        end
    end
    def createLocalFileCollection_
        @filesPerExtension = Hash.new{|h, k|h[k] = []}
        Dir.each(@rootDirectory) do |dir, fn|
            File.new(dir, fn).tap{|file|@filesPerExtension[file.extension] << file}
            nil
        end
    end
    def loadChildren_
    end
end

if __FILE__ == $0
    tree = Tree.new("/home/gfannes/gubg/d/gubg/Sequence.d")
    tree.each do |file|
        puts("#{file}")
    end
end
