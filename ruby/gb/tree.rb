#A Tree is a collection of files defined by a specific anchorPoint in a file system:
# * Step from anchorPoint towards / until "root.tree" is found
# * All files in all subdirectories from "root.tree" are part of the collection
# * "root.tree: can include other files collections or exclude other files
class Tree
    @@rootFileName = "root.tree"

    def initialize(anchorPoint)
        @anchorPoint = File.expand_path(anchorPoint)
        loadRoot_
        createLocalFileList_
        loadChildren_
    end

    private
    def loadRoot_
        #Find @rootFile: step down from @anchorPoint towards /, until "root.tree" is found
        @rootFile = nil
        directory = File.directory?(@anchorPoint) ? @anchorPoint : File.dirname(@anchorPoint)
        loop do
            tmpRootFile = File.expand_path(@@rootFileName, directory)
            if File.exist?(tmpRootFile)
                @rootFile = tmpRootFile
                break
            end
            nextDirectory = File.dirname
            raise("No #{@@rootFile} found") if nextDirectory == directory
        end
        #Load "root.tree"
        load(@rootFile)
    end
    def createLocalFileList_
    end
    def loadChildren_
    end
end

if __FILE__ == $0
    tree = Tree.new("/home/gfannes/gubg/d/gubg/Sequence.d")
    tree.each do |fileName|
        puts(fileName)
    end
end
