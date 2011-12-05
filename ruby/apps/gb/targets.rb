require("gubg/utils")

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

class Locations < Target
    attr_reader(:locations)
    def initialize
        super(state: :generated)
        @locations = [Dir.pwd]
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
        @roots = sources.first.locations.dup
        setState_(:generated, "The config contains #{@roots.length} roots")
    end
end
class Trees < Target
    attr_reader(:trees)
    def initialize
        super
    end
    def generate(sources)
        reWantedFile = /\.[ch]pp$/
        @trees = sources.first.roots.map do |base|
            files = []
            Dir.each(base) do |dir, fn|
                files << File.expand_path(fn, dir) if reWantedFile =~ fn
                :ok
            end
            files
        end
        setState_(:generated, "I can use #{@trees.flatten.length} files")
    end
end
class CppFiles < Target
    attr_reader(:files)
    def initialize
        super
        @files = []
    end
    def progressible?(sources)
        sources.any?{|s|Trees === s && s.state == :generated}
    end
    def generate(sources)
        hppFiles, trees, configs = *[HppFiles, Trees, Configs].map{|klass|sources.find{|s|klass === s}}
        if hppFiles.files.empty? and @files.empty?
            @files = configs.roots.map{|d|Dir[d+"/main.cpp"]}.flatten.slice!(0, 1)
            if @files.empty?
                setState_(:error, "Cannot find a main file")
            else
                setState_(:inprogress, "I found #{@files.length} main files")
            end
        else
            setState_(:halted, "I found #{@files.length} main files")
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
        cppFiles, trees = *[CppFiles, Trees].map{|klass|sources.find{|s|klass === s}}
        return setState_(:halted, "No cpp files present (yet)") if cppFiles.files.empty?
        #Search for sources we did not check for headers yet
        uncheckSources = cppFiles.files.reject{|f|@includesPerSource.has_key? f}
        return setState_(:halted, "No more sources to check") if uncheckSources.empty?
        uncheckSources.each{|cpp|@includesPerSource[cpp] = extractIncludes_ cpp}
        setState_(:inprogress, "Still checking")
    end
    private
    @@reInclude = /^\#include\s+["<](.+)[">]\s*/
    def extractIncludes_(cpp)
        puts("#{cpp}")
        String.loadLines(cpp).map{|l|l[@@reInclude, 1]}.uniq
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
