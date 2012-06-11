require("gubg/utils")
require("gubg/target")
require("gubg/cartouche")
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
    def setExtension_(name, type)
	    extPerType = case operatingSystem
			 when /^Linux/
				 {executable: ""}
			 when /^Windows/, /^Min/
				 {executable: "exe"}
			 end
	    raise("No info found to type #{type}") unless extPerType.has_key?(type)
	    name.dup.setExtension!(extPerType[type])
    end
end
module Platform
    def hostPlatform_
        if operatingSystem?(:linux)
            "pc-linux"
        elsif operatingSystem?(:windows)
            "pc-windows"
        else
            raise("Unknown host platform")
        end
    end
end

class TestAll
    include Platform
    attr_reader(:directory, :targetPlatform)
    def initialize(directory)
        @directory = directory
        @targetPlatform = hostPlatform_
    end
    def breakdown_
        defineScope(:testAll)
        configs = breakdown(Configs)
        tests = []
        Dir.each(@directory) do |dir, fn|
            filename = File.expand_path(fn, dir)
            case filename
            when /tests?\.cpp$/ then tests << {filename: filename}
            end
            :ok
        end
        log("Collected #{tests.length} tests:")
        log("#{tests*"\n"}")

        tests.each do |test|
            begin
                exe = breakdown(Executable.new(test[:filename]))
                test[:exe] = exe
                test[:state] = exe.state
            rescue
                test[:state] = :error
            end
        end

        tests.each do |test|
            if test[:state] == :ok and test.has_key?(:exe)
                run = breakdown(Run.new(test[:exe].executable))
                test[:run] = run
                test[:state] = run.state == :ok ? :ok : :runError
            end
        end


        log("")
        log("TEST REPORT")
        nrPerState = Hash.new(0)
        tests.each do |t|
            state = t[:state]
            puts("#{t[:filename]}".ljust(100) + "#{state}")
            nrPerState[state] += 1
        end
        nrPerState.each{|state, nr|log("state #{state}:\t#{nr}")}
        log("")
    end
end
class Executable
    include Executer
    include Platform
    attr_reader(:executable, :mainfile, :directory, :filestore, :state, :targetPlatform)
    def initialize(mainfile)
        @mainfile = File.expand_path(mainfile, nil, true)
        @directory = File.dirname(@mainfile)
        @filestore = $filestore
        @state = nil
        @cartouche = Cartouche.createFrom(file: @mainfile)
        info = @cartouche.getInfo({})
        @targetPlatform = info["platform.target"] || hostPlatform_
    end
    def breakdown_
        defineScope(:exe)
        objects = breakdown(ObjectFiles)
        if objects.failures > 0
            @state = :compileError
            return
        end
        link = breakdown(LinkSettings)
        config = breakdown(Configs)
        libraryPaths = link.libraryPaths.map{|lp|"-L#{lp}"}.join(" ")
        libraries = link.libraries.map{|l|"-l#{l}"}.join(" ")
        @executable = setExtension_(@mainfile, :executable)
        execute_("rm #{@executable}") if File.exist?(@executable)
        command = "#{config.linker} #{config.linkSettings} -o #{@executable} " + objects.objects.join(" ") + " " + libraryPaths + " " + libraries
        @state = (execute_(command) ? :ok : :linkError)
    end
end
class Run
    attr_reader(:state)
    def initialize(exe)
        @exe = exe
    end
    def breakdown_
        if !@exe
            @state = :error
            return
        end
        puts("")
        puts("*"*100)
        command = @exe
        puts(command.center(100, "*"))
        puts("*"*100)
        res = system(command)
        puts("*"*100)
        if res
            @state = :ok
        else
            @state = :error
            log("ERROR::Failure executing #{command} (#{$?})")
        end
    end
end
class ProgramArduino
    include Executer
    attr_reader(:state)
    def initialize(elf)
        @elf = elf
        @hex = "#{@elf}.hex"
    end
    def breakdown_
        cmd = "avr-objcopy -j .text -O ihex #{@elf} #{@hex}"
        raise("Could not convert elf into hex") unless execute_(cmd)
        cmd = "avrdude -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:#{@hex}"
        raise("Could not program hex into arduino") unless execute_(cmd)
        @state = :ok
    end
end

class ObjectFiles
    attr_reader(:objects, :failures)
    def breakdown_
        @objects = []
        sources = breakdown(Sources)
        compile = breakdown(CompileSettings)
        includePaths = compile.includePaths.map{|ip|"-I#{ip}"}*" "
        macros = compile.macros.map{|m|"-D#{m}"}*" "
        @failures = 0
        sources.files.each do |file|
            objectFile = breakdown(ObjectFile, file, macros, includePaths)
            if objectFile.state == :ok
                @objects << objectFile.file
            else
                @failures += 1
            end
        end
    end
end
class ObjectFile
    include Executer
    attr_reader(:file, :state)
    def initialize(source, macros, includePaths)
        @source, @macros, @includePaths = source, macros, includePaths
        @state = nil
    end
    def ObjectFile.key_(context, source, macros, includePaths)
        source.name
    end
    def breakdown_
        obj = @source.name.gsub(/\.c(pp)?$/, ".o")
        fi = FileInfo.new(obj)
        fmi = breakdown(FullMetaInfo, @source)
        config = breakdown(Configs)
        fi["recursive header digest"] = fmi.digest
        fi["macros"] = "#{@macros}"
        fi["include paths"] = "#{@includePaths}"
        begin
            context.filestore.create(fi) do |fn|
                unless execute_("#{config.compiler} #{config.compileSettings} -c #{@source} -o #{fn} #{@macros} #{@includePaths}")
                    raise("Failed to compile #{@source}")
                end
            end
            @file = context.filestore.name(fi)
            @state = :ok
        rescue
            @state = :error
        end
    end
    def to_s; @source.to_s end
end
class FullMetaInfo
    @@verbose = false
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
        log("Looking for all info for #{@source}")

        newFiles = [@source]
        while file = newFiles.shift
            log("Next file to process: #{file} #{file.class}") if @@verbose
            meta = breakdown(MetaInfo, file, @trees)
            @metaPerFile[file] = meta
            #Add the source look-alikes of the headers
            headers = meta.headers
            if headers.empty?
                log("No headers could be resolved from #{file}") if @@verbose
            else
                log("Headers: #{headers}") if @@verbose
                sources = findMatchingSources_(headers)
                log("Sources: #{sources}") if @@verbose
                (headers + sources).each do |file|
                    newFiles << file unless @metaPerFile.has_key?(file)
                end
            end
        end
    end
    def sourceFiles
        reCpp = /.c(pp)?$/
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
    @@verbose = false
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
        configs = breakdown(Configs)
        @fullMetaInfo = breakdown(FullMetaInfo, context.mainfile)
        @files = @fullMetaInfo.sourceFiles.uniq + configs.extrafiles.map{|fn|Tree::File.new(nil, fn)}
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
        @trees = configs.roots.map{|root|Tree.new(root, configs.wantedFiles)}
    end
end
class Configs
    attr_reader(:roots, :compiler, :compileSettings, :linker, :linkSettings, :includePaths, :libraryPaths, :libraries, :wantedFiles, :extrafiles)
    def initialize
        @roots = []
        @compiler = nil
        @compileSettings = nil
        @includePaths = []
        @linker = nil
        @linkSettings = nil
        @libraryPaths = []
        @libraries = []
        @wantedFiles = /\.[ch]pp$/
        @extrafiles = []
    end
    def breakdown_
        @roots << context.directory
        case context.targetPlatform
        when /^pc-/
            @roots << File.expand_path("cpp/libs/gubg", ENV["GUBG"])
            @roots << File.expand_path("cpp/icui", ENV["ICUI"])
            @compiler, @linker = "g++", "g++"
            @compileSettings = "-std=c++0x -O3"
            @linkSettings = "-std=c++0x"
            sfmlLibs = %w[sfml-graphics sfml-window sfml-audio sfml-system]
            boostLibs = %w[boost_thread boost_system boost_filesystem boost_regex boost_signals]
            case context.targetPlatform
            when "pc-linux"
                @includePaths << "/usr/include/cairomm-1.0"
                @includePaths << "/usr/include/cairo"
                @includePaths << "/usr/include/freetype2"
                @includePaths << "$HOME/sdks/libsigc++"
                @includePaths << "$HOME/sdks/SFML/include"
                @includePaths << "$HOME/sdks/wt/src"
                @includePaths << "$HOME/sdks/eigen"
                @libraryPaths << "$HOME/sdks/boost/lib"
                @libraryPaths << "$HOME/sdks/SFML/lib"
                @libraryPaths << "$HOME/sdks/wt/build/src"
                @libraryPaths << "$HOME/sdks/wt/build/src/http"
                sdlLibs = %w[SDL]
                cairoLibs = %w[cairomm-1.0]
                wtLibs = %w[wt wthttp]
                openglLibs = %w[GLU]
                @libraries += boostLibs + sdlLibs + cairoLibs + sfmlLibs + openglLibs + wtLibs
            when "pc-windows"
                @roots << File.expand_path("g:/src/cpp")
                @includePaths << "h:/software/boost_1_47_0"
                @includePaths << "h:/sdks/SFML/include"
                @libraryPaths << "h:/software/boost_1_47_0/stage/lib"
                @libraryPaths << "h:/sdks/SFML/lib"
                #Boost was built as such: ".\b2 toolset=gcc --build-type=complete stage"
                @libraries += boostLibs.map!{|l|"#{l}-mgw45-mt-1_47"} + sfmlLibs
            else
                raise("Unknown pc platform")
            end
        when "arduino"
            @roots << "/home/gfannes/sdks/Arduino/hardware/arduino/cores"
            @roots << "/home/gfannes/sdks/Arduino/hardware/arduino/variants/standard"
            @roots << "/home/gfannes/sdks/Arduino/libraries/Servo"
            @compiler, @linker = "avr-g++", "avr-g++"
            @compileSettings = "-g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22"
            @linkSettings = "-g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=22"
            @wantedFiles = /\.[ch](pp)?$/
            %w[main.cpp wiring.c wiring_digital.c WMath.cpp].each do |base|
                @extrafiles << "/home/gfannes/sdks/Arduino/hardware/arduino/cores/arduino/#{base}"
            end
            #@extrafiles << "/home/gfannes/sdks/Arduino/libraries/Servo/Servo.cpp"
            #            @includePaths << "$HOME/sdks/Arduino/hardware/arduino/cores/arduino"
            #            @includePaths << "$HOME/sdks/Arduino/hardware/arduino/variants/standard"
        else
            raise("Unknown targetPlatform #{context.targetPlatform}")
        end
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
