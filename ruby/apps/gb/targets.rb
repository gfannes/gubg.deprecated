require("gubg/utils")
require("gubg/target")
require("tree.rb")
require("set")
require("digest/md5")

module Executer
    def execute_(command)
        log("Executing command:\n#{command}")
        res = system(command)
        log("Result: #{res}")
        res
    end
    def addExtension_(name, type)
	    extPerType = case operatingSystem
			 when /^Linux/
				 {executable: nil}
			 when /^Windows/, /^Min/
				 {executable: "exe"}
			 end
	    raise("No info found to type #{type}") unless extPerType.has_key?(type)
	    name.addExtension(extPerType[type])
    end
end

class Executable
    include Executer
    attr_reader(:executable, :mainfile, :filestore)
    def initialize(mainfile)
        @mainfile = File.expand_path(mainfile, nil, true)
        @filestore = $filestore
    end
    def breakdown_
        defineScope(:exe)
        objects = breakdown(ObjectFiles)
        link = breakdown(LinkSettings)
        libraryPaths = link.libraryPaths.map{|lp|"-L#{lp}"}.join(" ")
        libraries = link.libraries.map{|l|"-l#{l}"}.join(" ")
        @executable = addExtension_("exe", :executable)
        execute_("rm #{@executable}") if File.exist?(@executable)
        command = "g++ -o #{@executable} " + objects.objects.join(" ") + " " + libraryPaths + " " + libraries
        execute_(command)
    end
end
class Run
    def initialize(exe)
        @exe = exe
    end
    def breakdown_
        puts("*"*100)
        command = "./#{@exe}"
        raise("Failure executing #{command}") if !system(command)
        puts("*"*100)
    end
end

class ObjectFiles
    attr_reader(:objects)
    @@reCpp = /\.cpp$/
        def breakdown_
            @objects = []
            sources = breakdown(Sources)
            compile = breakdown(CompileSettings)
            includePaths = compile.includePaths.map{|ip|"-I#{ip}"}*" "
            macros = compile.macros.map{|m|"-D#{m}"}*" "
            sources.files.each do |file|
                objectFile = breakdown(ObjectFile, file, macros, includePaths)
                @objects << objectFile.file
            end
        end
end
class ObjectFile
    include Executer
    attr_reader(:file)
    def initialize(source, macros, includePaths)
        @source, @macros, @includePaths = source, macros, includePaths
    end
    def ObjectFile.key_(context, source, macros, includePaths)
        source.name
    end
    def breakdown_
        obj = @source.name.gsub(/\.cpp$/, ".o")
        fi = FileInfo.new(obj)
        fmi = breakdown(FullMetaInfo, @source)
        fi["recursive header digest"] = fmi.digest
        fi["macros"] = "#{@macros}"
        fi["include paths"] = "#{@includePaths}"
        context.filestore.create(fi) do |fn|
            unless execute_("g++ -std=c++0x -O3 -c #{@source} -o #{fn} #{@macros} #{@includePaths}")
                raise("Failed to compile #{@source}")
            end
        end
        @file = context.filestore.name(fi)
    end
    def to_s; @source.to_s end
end
class FullMetaInfo
    def initialize(source)
        @source = source
    end
    def FullMetaInfo.key_(context, source)
        source
    end
    def breakdown_
        @metaPerFile = {}
        @trees = breakdown(Trees).trees

        @source = resolveFiles_(@source).first

        newFiles = [@source]
        while file = newFiles.shift
            log("Next file to process: #{file} #{file.class}")
            meta = breakdown(MetaInfo, file, @trees)
            @metaPerFile[file] = meta
            #Add the source look-alikes of the headers
            headers = meta.headers
            if headers.empty?
                log("No headers could be resolved from #{file}")
            else
                log("Headers: #{headers}")
                sources = findMatchingSources_(headers)
                log("Sources: #{sources}")
                (headers + sources).each do |file|
                    newFiles << file unless @metaPerFile.has_key?(file)
                end
            end
        end
    end
    def sourceFiles
        reCpp = /.cpp$/
            @metaPerFile.keys.select{|f|f.name =~ reCpp}
    end
    def metas
        @metaPerFile.values
    end
    def digest
        md5 = Digest::MD5.new
        md5 << String.loadBinary(@source.name)
        directlyUsedHeaders_.each do |header|
            md5 << String.loadBinary(header.name)
        end
        md5.digest
    end
    def to_s; @source.to_s end
    private
    def directlyUsedHeaders_
        headers = {}
        newHeaders = @metaPerFile[@source].headers
        until newHeaders.empty?
            tmp = []
            newHeaders.each do |header|
                unless headers.has_key?(header)
                    headers[header] = true
                    raise("Nothing is known about #{header}") unless @metaPerFile.has_key?(header)
                    tmp << @metaPerFile[header].headers
                end
            end
            newHeaders = tmp.flatten
        end
        headers.keys
    end
    def resolveFiles_(*files)
        files.flatten.map do |file|
            if Tree::File === file
                file
            else
                @trees.map{|tree|tree.find(file, :exact)}.compact.first
            end
        end.flatten
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
class MetaInfo
    @@verbose = true
    @@reInclude = /^\#include\s+["<](.+)[">]\s*/
        attr_reader :file, :refs
    def initialize(file, trees)
        @file, @trees = file, trees
    end
    def MetaInfo.key_(context, file, trees)
        file
    end
    def breakdown_
        @refs = String.loadLines(@file.name).map{|l|l[@@reInclude, 1]}.compact.uniq
        @headerPerRef = {}
        @refs.each do |ref|
            log("Searching for a match for include ref #{ref}") if @@verbose
            matches = @trees.map{|tree|tree.find(ref, :approx)}.flatten
            log("I found #{matches.length} matches: #{matches.map{|m|[m.name, m.class]}}") if @@verbose
            unless matches.empty?
                #Select _that_ match that looks the most like the original file.name, meaning it is somewhere close in a tree
                bestMatch = matches.sort do |x, y|
                    MetaInfo.stringEquality_(@file.name, x.name) <=> MetaInfo.stringEquality_(@file.name, y.name)
                end.last
                log("Best match for #{@file}: #{bestMatch}") if @@verbose
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
    def MetaInfo.stringEquality_(x, y)
        minLength = [x, y].map{|a|a.length}.min
        eq = -1
        loop do
            eq += 1
            break if eq >= minLength
            break if x[eq] != y[eq]
        end
        eq
    end
    def to_s; "#{@file}" end
end
class Sources
    attr_reader(:files)
    def breakdown_
        @fullMetaInfo = breakdown(FullMetaInfo, context.mainfile)
        @files = @fullMetaInfo.sourceFiles.uniq
    end
    def includePaths
        paths = Set.new
        @fullMetaInfo.metas.each do |meta|
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
    def to_s
        @files ? "#{@files.length} source files found" : ""
    end
end

class Trees
    attr_reader(:trees)
    def breakdown_
        configs = breakdown(Configs)
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
        boostLibs = %w[boost_thread boost_system boost_filesystem boost_regex]
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
    def breakdown_
        #This is still a short cut
        gubgRoot = File.expand_path("cpp/libs/gubg", ENV["GUBG"])
        @roots = [File.dirname(context.mainfile), gubgRoot]
    end
end
class CompileSettings
    attr_reader(:includePaths, :macros)
    def breakdown_
        configs = breakdown(Configs)
        sources = breakdown(Sources)
        @includePaths = sources.includePaths + configs.includePaths
        @macros = []
        case operatingSystem
        when "MinGW"
            @macros << "MINGW32"
        end
    end
end
class LinkSettings
    attr_reader(:libraryPaths, :libraries)
    def breakdown_
        configs = breakdown(Configs)
        @libraryPaths = configs.libraryPaths
        @libraries = configs.libraries
    end
end
