require("gubg/utils")
require("gubg/target")
require("tree.rb")
require("set")

class Targett
    attr_reader(:state, :msg, :warnings)
    def initialize
        @warnings = []
    end
    def setState(state, msg)
        @state, @msg = state, msg
        puts("state: #{@state}::#{@msg}")
        @state
    end
    def state?(wantedState)
        @state == wantedState
    end
    def progressible?
        false
    end
    def execute_(command)
        puts("Executing \"#{command}\"")
        system(command)
    end
    def warning(msg)
        @warnings << msg
    end
end

class Location < Targett
    attr_reader(:location)
    include Target
    def initialize
        super
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
    attr_reader(:compiler, :linker, :roots, :includePaths, :libraryPaths, :libraries)
    include Target
    def initialize
        super
        defineDependencies(location: Location)
        @compiler = "g++ -std=c++0x"
        @linker = "g++ -std=c++0x"
        @includePaths = []
        @libraryPaths = []
        boostLibs = %w[boost_thread boost_system boost_filesystem]
        if operatingSystem =~ /^Linux/
            @includePaths << "/usr/include/cairomm-1.0"
            @includePaths << "/usr/include/cairo"
            @includePaths << "/usr/include/freetype2"
            @includePaths << "$HOME/sdks/libsigc++"
            @libraryPaths << "$HOME/sdks/boost/lib"
            @libraries = boostLibs
        else
            @includePaths << "h:/software/boost_1_47_0"
            @libraryPaths << "h:/software/boost_1_47_0/stage/lib"
            #Boost was built as such: ".\b2 toolset=gcc --build-type=complete stage"
            @libraries = boostLibs.map!{|l|"#{l}-mgw45-mt-1_47"}
        end
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
        super
        defineDependencies(configs: Configs)
    end
    def generate_
        configs = getTarget(:configs)
        reWantedFiles = /\.[ch]pp$/
            @trees = configs.roots.map{|root|Tree.new(root, reWantedFiles)}
        setState(:generated, "I can use #{@trees.map{|t|t.files}.flatten.length} files in #{@trees.length} trees")
    end
end
class Sources < Targett
    include Target
    def initialize
        super
        defineDependencies(references: References, trees: Trees, configs: Configs)
        @files = Set.new
        @staging = []
        @sourceInfoPerHeader = {}
    end
    def add(file)
        @staging << file
    end
    def files
        @files.to_a
    end
    def progressible?
        getTarget(:trees).state?(:generated)
    end
    def generate_
        references, trees, configs = getTargets(:references, :trees, :configs)
        if @staging.empty?
            if generationState == :halted and [:halted, :generated].any?{|s|references.state?(s)}
                if @files.empty?
                    return setState(:error, "No files found for compilation")
                else
                    return setState(:generated, "No more references to resolve either, we are finished")
                end
            else
                return setState(:halted, "No files found in staging")
            end
        end
        while file = @staging.shift
            case file
            when String
                #We search for the specified file in the trees
                found = false
                trees.trees.each do |tree|
                    if f = tree.find(file, :exact)
                        add(f)
                        found = true
                        break
                    end
                end
                warning("Could not find \"#{file}\"") if !found
            when Tree::File
                unless @files.include?(file)
                    references.add(file)
                    @files << file
                end
            else
                return setState(:error, "Cannot handle a file of type #{file.class}")
            end
        end
        setState(:generating, "Staging is processed")
    end
end
class References < Targett
    include Target
    def initialize
        super
        defineDependencies(sources: Sources, trees: Trees)
        @infoPerFile = {}
        @staging = []
    end
    def add(file)
        @staging << file
    end
    def progressible?
        getTarget(:trees).state?(:generated)
    end
    def includePaths
        paths = Set.new
        @infoPerFile.each do |file, info|
            info[:deps].each do |dep|
                if dep.has_key?(:file)
                    path = dep[:file].name.dup
                    path[dep[:ref]] = ""
                    paths << path
                end
            end
        end
        paths.to_a
    end
    def generate_
        sources, trees = getTargets(:sources, :trees)
        if @staging.empty?
            if generationState == :halted and [:halted, :generated].any?{|s|sources.state?(s)}
                return setState(:generated, "No more sources to check either, we are finished")
            else
                return setState(:halted, "No files found in staging")
            end
        end
        while file = @staging.shift
            return setState(:error, "I don't know how to extract references from #{file.class}") if !(Tree::File === file)
            unless @infoPerFile.has_key?(file)
                refs = extractIncludeRefs_(file.name)
                deps = refs.map do |ref|
                    dep = {ref: ref}
                    puts("Searching for a match for include ref #{ref}")
                    matches = trees.trees.map{|tree|tree.find(ref, :approx)}.flatten
                    puts("I found #{matches.length} matches: #{matches.map{|m|m.name}}")
                    unless matches.empty?
                        #Select _that_ match that looks the most like the original file.name, meaning it is somewhere close in a tree
                        bestMatch = matches.sort do |x, y|
                            stringEquality_(file.name, x) <=> stringEquality_(file.name, y)
                        end.last
                        puts("Best match for #{file}: #{bestMatch}")
                        dep[:file] = bestMatch
                        add(bestMatch)
                    end
                    dep
                end
                @infoPerFile[file] = {name: file.name, deps: deps}
                #For headers, we add the source lookalike to sources
                reHeader = /\.hp*$/
                    if file.name =~ reHeader
                        sources.add(file.name.gsub(reHeader, ".cpp"))
                    end
            end
        end
        setState(:generating, "Staging is processed")
    end
    private
    def References.stringEquality_(x, y)
        minLength = [x, y].min
        eq = -1
        loop do
            eq += 1
            break if eq >= minLength
            break if x[eq] != y[eq]
        end
        eq
    end
    @@reInclude = /^\#include\s+["<](.+)[">]\s*/
        def extractIncludeRefs_(cpp)
            String.loadLines(cpp).map{|l|l[@@reInclude, 1]}.compact.uniq
        end
end
class CompileSettings < Targett
    attr_reader(:includePaths)
    include Target
    def initialize
        super
        defineDependencies(configs: Configs, trees: Trees, references: References, sources: Sources)
    end
    def generate_
        references, configs = getTargets(:references, :configs)
        @includePaths = references.includePaths + configs.includePaths
        setState(:generated, "I will use the following include paths: #{@includePaths.join('|')}")
    end
end
class LinkSettings < Targett
    attr_reader(:libraryPaths, :libraries)
    include Target
    def initialize
        super
        defineDependencies(configs: Configs)
    end
    def generate_
        configs = getTargets(:configs)
        @libraryPaths = configs.libraryPaths
        @libraries = configs.libraries
        setState(:generated, "I will link against the following libraries: #{@libraries.join('|')}")
    end
end
class ObjectFiles < Targett
    attr_reader(:objects)
    include Target
    def initialize
        super
        defineDependencies(compile: CompileSettings, sources: Sources)
        @objects = []
    end
    def generate_
        sources, compile = getTargets(:sources, :compile)
        includePaths = compile.includePaths.map{|ip|"-I#{ip}"}.join(" ")
        sources.files.each do |cppFile|
            objectFile = cppFile.name.gsub(/\.cpp$/, ".o")
            if !execute_("g++ -std=c++0x -O3 -c #{cppFile} -o #{objectFile} #{includePaths}")
                return setState(:error, "Failed to compile #{cppFile}")
            end
            @objects << objectFile
        end
        setState(:generated, "I linked #{@objects.length} objects")
    end
end
class Executables < Targett
    attr_reader(:executable)
    include Target
    def initialize
        super
        defineDependencies(link: LinkSettings, objects: ObjectFiles)
        @executable = ""
    end
    def generate_
        objects, link = getTargets(:objects, :link)
        libraryPaths = link.libraryPaths.map{|lp|"-L#{lp}"}.join(" ")
        libraries = link.libraries.map{|l|"-l#{l}"}.join(" ")
        system("rm exe")
        command = "g++ -o exe " + objects.objects.join(" ") + " " + libraryPaths + " " + libraries
        puts("Link command: #{command}")
        system(command)
        setState(:generated, "I compiled #{@executable}")
    end
end
