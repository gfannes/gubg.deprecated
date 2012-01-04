require("gubg/utils")
require("gubg/target")
require("tree.rb")

class Targett
    attr_reader(:state, :msg)
    def setState(state, msg)
        @state, @msg = state, msg
        @state
    end
    def state?(wantedState)
        @state == wantedState
    end
    def progressible?
        false
    end
end

class Location < Targett
    attr_reader(:location)
    include Target
    def initialize
        defineDependencies()
    end
    def set(loc)
        @location = loc
    end
    def generate_
        if @location.nil?
            setState(:error, "You have to set the location")
        else
            setState(:generated, "Location is #{@location}")
        end
    end
end
class Configs < Targett
    attr_reader(:compiler, :linker, :roots)
    include Target
    def initialize
        defineDependencies(location: Location)
        @compiler = "g++ -std=c++0x"
        @linker = "g++ -std=c++0x"
    end
    def generate_
        location = getTarget(:location)
        #This is still a short cut
        gubgRoot = File.expand_path("cpp/libs/gubg", ENV["GUBG"])
        @roots = [location.location.dup, gubgRoot]
        str = @roots.join("\n")
        setState(:generated, "The config contains the following roots (#{@roots.length}):\n#{str}")
    end
end
class Trees < Targett
    attr_reader(:trees)
    include Target
    def initialize
        defineDependencies(configs: Configs)
    end
    def generate_
        configs = getTarget(:configs)
        reWantedFiles = /\.[ch]pp$/
            @trees = configs.roots.map{|root|Tree.new(root, reWantedFiles)}
        setState(:generated, "I can use #{@trees.map{|t|t.files}.flatten.length} files in #{@trees.length} trees")
    end
end
class CppFiles < Targett
    attr_reader(:files)
    attr_accessor(:starter)
    include Target
    def initialize
        defineDependencies(hppFiles: HppFiles, trees: Trees, configs: Configs)
        @files = []
        @sourcePerIncludePerSource = Hash.new{|h, k|h[k] = {}}
    end
    def progressible?
        getTarget(:trees).state?(:generated)
    end
    def generate_
        hppFiles, trees, configs = getTargets(:hppFiles, :trees, :configs)
        case generationState
        when :firstTime
            #We search for the specified file in the trees
            trees.trees.each do |tree|
                if file = tree.find(@starter)
                    @files << file
                    break
                end
            end
            return setState(:error, "The starter #{@starter} could not be found") if @files.empty?
            setState(:generating, "I will start with #{@files.length} files for compilation")
        when :generating
            if hppFiles.state?(:halted)
                setState(:halted, "The headers are not making any progress anymore, I will halt too")
            else
                #Check for new includes
                uncheckIncludesPerSource = 
                    setState(:generating, "I have currently selected #{@files.length} files for compilation")
            end
        when :halted
            if [:halted, :generated].include?(hppFiles.state)
                str = @files.join("\n")
                setState(:generated, "I will compile the following files (#{@files.length}):\n#{str}")
            else
                setState(:generating, "The headers are still making progress, I will wait some more")
            end
        end
    end
end
class HppFiles < Targett
    attr_reader(:files)
    include Target
    def initialize
        defineDependencies(cppFiles: CppFiles, trees: Trees)
        @files = []
        @includesPerSource = Hash.new{|h, k|h[k] = []}
    end
    def progressible?
        getTarget(:trees).state?(:generated)
    end
    def generate_
        cppFiles, trees = getTargets(:cppFiles, :trees)
        if cppFiles.files.empty?
            return setState(:generated, "No cpp files present") if generationState?(:halted)
            return setState(:halted, "No cpp files present (yet)")
        else
            #Search for sources we did not check for headers yet
            uncheckSources = cppFiles.files.reject{|f|@includesPerSource.has_key?(f)}
            if uncheckSources.empty?
                if generationState?(:halted)
                    str = ""
                    @includesPerSource.each do |s, is|
                        str += "#{s.name} includes " + is.join(", ")
                    end
                    return setState(:generated, "No more cpp files to check:\n#{str}")
                end
                return setState(:halted, "No more sources to check")
            end
            uncheckSources.each{|cpp|@includesPerSource[cpp] = extractIncludes_(cpp.name)}
            setState(:generating, "Still checking")
        end
    end
    private
    @@reInclude = /^\#include\s+["<](.+)[">]\s*/
        def extractIncludes_(cpp)
            String.loadLines(cpp).map{|l|l[@@reInclude, 1]}.compact.uniq
        end
end
class CompileSettings < Targett
    attr_reader(:includePaths)
    include Target
    def initialize
        defineDependencies(configs: Configs, trees: Trees, hppFiles: HppFiles, cppFiles: CppFiles)
    end
    def generate_
        @includePaths = []
        setState(:generated, "I will use the following include paths: #{@includePaths.join('|')}")
    end
end
class LinkSettings < Targett
    attr_reader(:libraryPaths, :libraries)
    include Target
    def initialize
        defineDependencies(configs: Configs)
    end
    def generate_
        @libraryPaths = []
        @libraries = []
        setState(:generated, "I will link against the following libraries: #{@libraries.join('|')}")
    end
end
class ObjectFiles < Targett
    attr_reader(:objects)
    include Target
    def initialize
        defineDependencies(compile: CompileSettings, cppFiles: CppFiles)
        @objects = []
    end
    def generate_
        setState(:generated, "I will link #{@objects.length} objects")
    end
end
class Executables < Targett
    attr_reader(:executable)
    include Target
    def initialize
        defineDependencies(link: LinkSettings, objects: ObjectFiles)
        @executable = ""
    end
    def generate_
        setState(:generated, "I will compile #{@executable}")
    end
end
