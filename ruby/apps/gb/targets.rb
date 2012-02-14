require("gubg/utils")
require("gubg/target")
require("tree.rb")
require("set")

module Executer
    def execute_(command)
        puts("Executing command #{command}")
        system(command)
    end
end

class Executable
    attr_reader(:executable, :mainfile)
    def initialize(mainfile)
        @mainfile = File.expand_path(mainfile)
    end
    def generate_
        defineScope(:exe)
        objects = generate(ObjectFiles)
        link = generate(LinkSettings)
        libraryPaths = link.libraryPaths.map{|lp|"-L#{lp}"}.join(" ")
        libraries = link.libraries.map{|l|"-l#{l}"}.join(" ")
        @executable = "exe"
        system("rm #{@executable}")
        command = "g++ -o #{@executable} " + objects.objects.join(" ") + " " + libraryPaths + " " + libraries
        puts("Link command: #{command}")
        system(command)
    end
end
class Run
    def initialize(exe)
        @exe = exe
    end
    def generate_
        command = "./#{@exe}"
        raise("Failure executing #{command}") if !system(command)
    end
end

class ObjectFiles
    include Executer
    attr_reader(:objects)
    @@reCpp = /\.cpp$/
    def generate_
        @objects = []
        sources = generate(Sources)
        compile = generate(CompileSettings)
        includePaths = compile.includePaths.map{|ip|"-I#{ip}"}.join(" ")
        sources.files.each do |file|
            case file
            when @@reCpp
                objectFile = file.gsub(/\.cpp$/, ".o")
                if !execute_("g++ -std=c++0x -O3 -c #{file} -o #{objectFile} #{includePaths}")
                    raise("Failed to compile #{file}")
                end
                @objects << objectFile
            end
        end
    end
end
class Sources
    def generate_
        @metaPerFile = {}
        @trees = generate(Trees).trees

        newFiles = resolveFiles_(context.mainfile)
        while file = newFiles.shift
            puts("Next file to process: #{file}")
            meta = generate(Meta.new(file, @trees))
            @metaPerFile[file] = meta
            #Add the source look-alikes of the headers
            headers = meta.headers
            puts("Headers: #{headers}")
            sources = findMatchingSources_(headers)
            puts("Sources: #{sources}")
            (headers + sources).each do |file|
                newFiles << file unless @metaPerFile.has_key?(file)
            end
        end
    end
    def files
        @metaPerFile.keys.map{|f|f.name}.uniq
    end
    def includePaths
        paths = Set.new
        @metaPerFile.each do |file, meta|
            meta.refs.each do |ref|
                if header = meta.headerMatching(ref)
                    path = header.name.dup
                    path[ref] = ""
                    paths << path
                end
            end
        end
        paths.to_a
    end
    private
    def resolveFiles_(*files)
        files.flatten.map{|file|@trees.map{|tree|tree.find(file, :exact)}}.flatten
    end
    class Meta
        @@verbose = false
        @@reInclude = /^\#include\s+["<](.+)[">]\s*/
            attr_reader :file, :refs
        def initialize(file, trees)
            @file, @trees = file, trees
        end
        def generate_
            puts("Generating meta info for #{@file}")
            @refs = String.loadLines(@file.name).map{|l|l[@@reInclude, 1]}.compact.uniq
            @headerPerRef = {}
            @refs.each do |ref|
                puts("Searching for a match for include ref #{ref}") if @@verbose
                matches = @trees.map{|tree|tree.find(ref, :approx)}.flatten
                puts("I found #{matches.length} matches: #{matches.map{|m|m.name}}") if @@verbose
                unless matches.empty?
                    #Select _that_ match that looks the most like the original file.name, meaning it is somewhere close in a tree
                    bestMatch = matches.sort do |x, y|
                        stringEquality_(@file.name, x) <=> stringEquality_(@file.name, y)
                    end.last
                    puts("Best match for #{@file}: #{bestMatch}") if @@verbose
                    @headerPerRef[ref] = bestMatch
                end
            end
        end
        def headers
            @refs.map{|ref|@headerPerRef[ref]}.compact
        end
        def headerMatching(ref)
            @headerPerRef[ref]
        end
        def Meta.stringEquality_(x, y)
            minLength = [x, y].min
            eq = -1
            loop do
                eq += 1
                break if eq >= minLength
                break if x[eq] != y[eq]
            end
            eq
        end
    end
    @@reHeader = /\.hp*$/
        def findMatchingSources_(headers)
            sources = []
            headers.each do |header|
                sources << resolveFiles_(header.name.gsub(@@reHeader, ".cpp")) if header.name =~ @@reHeader
            end
            sources.flatten.compact
        end
end
class Trees
    attr_reader(:trees)
    def generate_
        configs = generate(Configs)
        reWantedFiles = /\.[ch]pp$/
            @trees = configs.roots.map{|root|Tree.new(root, reWantedFiles)}
    end
end
class Configs
    attr_reader(:compiler, :linker, :roots, :includePaths, :libraryPaths, :libraries)
    def initialize
        @compiler = "g++ -std=c++0x"
        @linker = "g++ -std=c++0x"
        @includePaths = []
        @libraryPaths = []
        boostLibs = %w[boost_thread boost_system boost_filesystem]
        sdlLibs = %w[SDL]
        cairoLibs = %w[cairomm-1.0]
        if operatingSystem =~ /^Linux/
            @includePaths << "/usr/include/cairomm-1.0"
            @includePaths << "/usr/include/cairo"
            @includePaths << "/usr/include/freetype2"
            @includePaths << "$HOME/sdks/libsigc++"
            @libraryPaths << "$HOME/sdks/boost/lib"
            @libraries = boostLibs + sdlLibs + cairoLibs
        else
            @includePaths << "h:/software/boost_1_47_0"
            @libraryPaths << "h:/software/boost_1_47_0/stage/lib"
            #Boost was built as such: ".\b2 toolset=gcc --build-type=complete stage"
            @libraries = boostLibs.map!{|l|"#{l}-mgw45-mt-1_47"}
        end
    end
    def generate_
        #This is still a short cut
        gubgRoot = File.expand_path("cpp/libs/gubg", ENV["GUBG"])
        @roots = [File.dirname(context.mainfile), gubgRoot]
    end
end
class CompileSettings
    attr_reader(:includePaths)
    def generate_
        configs = generate(Configs)
        sources = generate(Sources)
        @includePaths = sources.includePaths + configs.includePaths
    end
end
class LinkSettings
    attr_reader(:libraryPaths, :libraries)
    def generate_
        configs = generate(Configs)
        @libraryPaths = configs.libraryPaths
        @libraries = configs.libraries
    end
end
