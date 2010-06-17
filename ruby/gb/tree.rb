#A Tree is a collection of files defined by a specific anchorPoint in a file system:
# * Step from anchorPoint towards "/" until "root.tree" is found
# * All files in all subdirectories from "root.tree" are part of the collection
# * "root.tree: can include other files collections or exclude other files

require("gb/root")
require("gubg/utils")

class Tree
    @@rootFileName = "root.tree"

    attr(:root, true)
    attr(:anchorPoint, true)
    attr(:baseDirectory, true)
    def initialize(anchorPoint)
        @anchorPoint = ::File.expand_path(anchorPoint)
        if ::File.directory?(@anchorPoint)
            ::File.expand_path(@@rootFileName, @anchorPoint).tap do |ap|
                @anchorPoint = ap if ::File.exist?(ap)
            end
        end
        if ::File.directory?(@anchorPoint)
            @baseDirectory = @anchorPoint
        else
            @baseDirectory = ::File.dirname(@anchorPoint)
        end
        loadRoot_
        createLocalFileCollection_
        loadSubtrees_
    end
    def Tree.rootFileName
        @@rootFileName
    end

    def root?
        @@rootFileName == ::File.basename(@anchorPoint)
    end
    def name
        @root.name || ::File.basename(@baseDirectory)
    end

    def each(&block)
        @filesPerExtension.each do |extension, files|
            files.each{|file|block.call(file)}
        end
        @subtrees.each do |subtree|
            puts("\nSubtree")
            subtree.each(block)
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
            directory = ::File.dirname(directory).tap do |nextDirectory|
                raise("No #{@@rootFile} found") if nextDirectory == directory
            end
        end
        #Load "root.tree"
        puts("rootFilePath = #{@rootFilePath}")
        @root = Root.createFromFile(@rootFilePath)
    end
    class File
        attr(:directory, true)
        attr(:filename, true)
        def initialize(directory, filename)
            @directory, @filename = directory, filename
            @extension = ::File.extname(@filename)
        end
        def File.createFromFilepath(filepath)
            File.new(::File.dirname(filepath), ::File.basename(filepath))
        end
        def extension
            @extension
        end
        def filepath
            ::File.expand_path(@filename, @directory)
        end
        alias to_s filepath
    end
    def createLocalFileCollection_
        @filesPerExtension = Hash.new{|h, k|h[k] = []}
        Dir.each(@rootDirectory) do |dir, fn|
            File.new(dir, fn).tap{|file|@filesPerExtension[file.extension] << file}
            nil
        end
    end
    def loadSubtrees_
        @subtrees = @root.subtrees.collect do |subtree|
            Root.new(subtree)
        end
    end
end

if __FILE__ == $0
    tree = Tree.new("/home/gfannes/gubg/d/gubg/Sequence.d")
    tree.each do |file|
        puts("#{file}")
    end
end
