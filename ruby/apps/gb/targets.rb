require("gubg/utils")

STATES = [:ungenerated, :inprogress, :halted, :generated, :depleated]
class Target
    attr_reader(:state)
    def initialize(na = {state: nil})
        @state = na[:state] || :ungenerated
    end
    def progressible?(sources)
        false
    end
    def setState(wanted)
        @state = wanted if wanted == :generated and @state == :halted
    end
    def to_s
        "#{self.class}(#{@state})"
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
        @state = :generated
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
        end
        @state = :generated
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
        if hppFiles.files.empty?
            @files = configs.roots.map{|d|Dir[d+"/main.cpp"]}.flatten.slice!(0, 1)
            @state = :inprogress
        else
            @state = :halted
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
        return if cppFiles.files.empty?
        #Search for sources we did not check for headers yet
        uncheckSources = cppFiles.files.reject{|f|@includesPerSource.has_key? f}
        return @state = :halted if uncheckSources.empty?
        uncheckSources.each{|cpp|@includesPerSource[cpp] = extractIncludes_ cpp}
        @state = :inprogress
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
        @state = :generated
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
        @state = :generated
    end
end
class ObjectFiles < Target
    def initialize
        super
    end
    def generate(sources)
        @state = :generated
    end
end
class Executables < Target
    def initialize
        super
    end
    def generate(sources)
        @state = :generated
    end
end
