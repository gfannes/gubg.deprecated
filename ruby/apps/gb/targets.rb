require("gubg/utils")
require("tree.rb")

STATES = [:ungenerated, :inprogress, :halted, :generated, :depleated, :error]
class Target
    attr_reader(:state, :explanation)
    def initialize(na = {state: nil})
        @state = na[:state] || :ungenerated
        @explanation = ""
    end
    def progressible?(sources)
        false
    end
    def setState(s, expl)
        raise("State should be part of #{STATES}, but it is #{s}") if !STATES.include?(s)
        setState_(s, expl) if s == :generated and @state == :halted
    end
    def to_s
        "#{self.class}(#{@state})"
    end
    private
    def setState_(s, expl)
        @state, @explanation = s, expl
    end
end

def extractTargets(sources, *klasses)
        res = klasses.map{|klass|sources.find{|s|klass === s}}
        return res.first if res.length == 1
        return *res
end

class Location < Target
    attr_reader(:location)
    def initialize
        super
    end
    def set(loc)
        @location = loc
        setState_(:generated, "Location is #{@location}")
    end
end
class Configs < Target
    attr_reader(:compiler, :linker, :roots)
    def initialize
        super
        @compiler = "g++ -std=c++0x"
        @linker = "g++ -std=c++0x"
    end
    def generate(sources)
        location = extractTargets(sources, Location)
        #This is still a short cut
        gubgRoot = File.expand_path("cpp/libs/gubg", ENV["GUBG"])
        @roots = [location.location.dup, gubgRoot]
        str = @roots.join("\n")
        setState_(:generated, "The config contains the following roots (#{@roots.length}):\n#{str}")
    end
end
class Trees < Target
    attr_reader(:trees)
    def initialize
        super
    end
    def generate(sources)
        configs = extractTargets(sources, Configs)
        reWantedFiles = /\.[ch]pp$/
        @trees = configs.roots.map{|root|Tree.new(root, reWantedFiles)}
        setState_(:generated, "I can use #{@trees.map{|t|t.files}.flatten.length} files in #{@trees.length} trees")
    end
end
class CppFiles < Target
    attr_reader(:files)
    attr_accessor(:starter)
    def initialize
        super
        @files = []
        @sourcePerInclude = {}
    end
    def progressible?(sources)
        sources.any?{|s|Trees === s && s.state == :generated}
    end
    def generate(sources)
        hppFiles, trees, configs = extractTargets(sources, HppFiles, Trees, Configs)
        case state
        when :ungenerated
            #We search for the specified file in the trees
            trees.trees.each do |tree|
                if file = tree.find(@starter)
                    @files << file
                    break
                end
            end
            return setState_(:error, "The starter #{@starter} could not be found") if @files.empty?
            setState_(:inprogress, "I will start with #{@files.length} files for compilation")
        when :inprogress
            if hppFiles.state == :halted
                setState_(:halted, "The headers are not making any progress anymore, I will halt too")
            else
                setState_(:inprogress, "I have currently selected #{@files.length} files for compilation")
            end
        when :halted
            if [:halted, :generated].include?(hppFiles.state)
                str = @files.join("\n")
                setState_(:generated, "I will compile the following files (#{@files.length}):\n#{str}")
            else
                setState_(:inprogress, "The headers are still making progress, I will wait some more")
            end
        end
    end
end
class HppFiles < Target
    attr_reader(:files)
    def initialize
        super
        @files = []
        @includesPerSource = Hash.new{|h, k|h[k] = []}
    end
    def progressible?(sources)
        sources.any?{|s|Trees === s && s.state == :generated}
    end
    def generate(sources)
        cppFiles, trees = *extractTargets(sources, CppFiles, Trees)
        if cppFiles.files.empty?
            return setState_(:generated, "No cpp files present") if state == :halted
            return setState_(:halted, "No cpp files present (yet)")
        else
            #Search for sources we did not check for headers yet
            uncheckSources = cppFiles.files.reject{|f|@includesPerSource.has_key?(f)}
            if uncheckSources.empty?
                if state == :halted
                    str = ""
                    @includesPerSource.each do |s, is|
                        str += "#{s.name} includes " + is.join(", ")
                    end
                    return setState_(:generated, "No more cpp files to check:\n#{str}")
                end
                return setState_(:halted, "No more sources to check")
            end
            uncheckSources.each{|cpp|@includesPerSource[cpp] = extractIncludes_(cpp.name)}
            setState_(:inprogress, "Still checking")
        end
    end
    private
    @@reInclude = /^\#include\s+["<](.+)[">]\s*/
    def extractIncludes_(cpp)
        String.loadLines(cpp).map{|l|l[@@reInclude, 1]}.compact.uniq
    end
end
class CompileSettings < Target
    attr_reader(:includePaths)
    def initialize
        super
    end
    def generate(sources)
        @includePaths = []
        setState_(:generated, "I will use the following include paths: #{@includePaths.join('|')}")
    end
end
class LinkSettings < Target
    attr_reader(:libraryPaths, :libraries)
    def initialize
        super
    end
    def generate(sources)
        @libraryPaths = []
        @libraries = []
        setState_(:generated, "I will link against the following libraries: #{@libraries.join('|')}")
    end
end
class ObjectFiles < Target
    attr_reader(:objects)
    def initialize
        super
        @objects = []
    end
    def generate(sources)
        setState_(:generated, "I will link #{@objects.length} objects")
    end
end
class Executables < Target
    attr_reader(:executable)
    def initialize
        super
        @executable = ""
    end
    def generate(sources)
        setState_(:generated, "I will compile #{@executable}")
    end
end
