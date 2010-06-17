require("gb/commands")
require("gb/dependency")
require("gubg/utils")
require("gubg/filestore")

def selectCommandFactory(tree, operation)
    res = nil
    case operation
    when :compile
        countPerExtension = Hash.new{|h, k|h[k] = 0}
        tree.each do |file|
            countPerExtension[file.extension] += 1
        end
        mostFrequentExtension = nil
        countPerExtension.eachDec do |extension, count|
            if [".d", ".cpp"].include?(extension)
                mostFrequentExtension = extension
                break
            end
        end
        case mostFrequentExtension
        when ".d"
            raise("I cannot combine d and cpp yet") if countPerExtension[".cpp"] > 0
            res = DCompileFactory.new
        when ".cpp"
            raise("I cannot combine cpp and d yet") if countPerExtension[".d"] > 0
            res = CPPCompileFactory.new
        else
            raise("Could not find either d or cpp source files")
        end
    when :link
        res = DLinkFactory.new
    else
        raise("Unknown operation #{operation}")
    end
    res
end

class Factory
    def initialize(language)
        @language = case language
                    when :d
                        DLanguage.new
                    when :cpp
                        CPPLanuage.new
                    else
                        language
                    end
    end

    #Finds all referred modules (recursively) and return file information on them
    def findInfoPerModule(filepath, tree)
        puts("Finding module info for #{filepath}") if $verbose
        infoPerModule = {}
        modules2Check = @language.modulesPerFile(filepath)
        while !modules2Check.empty?
            newModules2Check = []
            modules2Check.uniq.each do |mod|
                if info = @language.moduleInfo(mod, tree)
                    @language.modulesPerFile(info.filepath).each do |newMod|
                        newModules2Check << newMod if !infoPerModule.has_key?(newMod)
                    end
                end
                infoPerModule[mod] = info
            end
            modules2Check = newModules2Check
        end
        infoPerModule
    end
end

class DLinkFactory < Factory
    def initialize
        @fileStore = FileStore.new("/tmp/gb")
        super(:d)
    end
    def createCommands(tree)
        if tree.root?
            #Create a library
            dirLib = File.expand_path("lib", tree.baseDirectory)
            archiveExtension = case operatingSystem
                               when "Linux"
                                   archiveExtension = ".a"
                               when Collection.new("MinGW", "Windows")
                                   archiveExtension = ".lib"
                               end
            libName = File.expand_path("lib#{tree.name}" + archiveExtension, dirLib)
            fileInfo = FileInfo.new(libName)
            fileInfo[:libName] = libName
            fileInfo[:objects] = CompileCommand.getObjectFiles(:d, tree)
            [ArchiveCommand.new(fileInfo, @fileStore)]
        else
            #Create an executable and execute it
            execName = tree.anchorPoint + ".exec"
            fileInfo = FileInfo.new(execName)
            fileInfo[:execName] = execName
            fileInfo[:objectFiles] = CompileCommand.getObjectFiles(:d, tree)
            fileInfo[:language] = :d
            [].tap do |linkerSettings|
                findInfoPerModule(tree.anchorPoint, tree).each do |mod, info|
                    tree.root.language[:d][:linker].each do |when_, setting|
                        case when_
                        when :always
                            linkerSettings << setting
                        when Regexp, String
                            linkerSettings << setting if mod[when_]
                        end
                    end
                end
                fileInfo[:settings] = linkerSettings.compact.uniq.sort.join(" ")
            end
            [LinkCommand.new(fileInfo, @fileStore), ExecuteCommand.new(execName)]
        end
    end
end

class CompileFactory < Factory
    @@objectFilesPerLanguangePerTree = Hash.new{|h, k|h[k] = Hash.new{|hh, kk|hh[kk] = []}}

    def initialize(language)
        @fileStore = FileStore.new("/tmp/gb")
        super(language)
    end
    def CompileCommand.getObjectFiles(language, tree)
        @@objectFilesPerLanguangePerTree[language][tree]
    end

    def createCommands(tree)
        commands = []
        tree.each do |file|
            if fileInfo = createFileInfoForObjectFile_(file, tree)
                commands << CompileCommand.new(fileInfo, @fileStore)
                @@objectFilesPerLanguangePerTree[fileInfo[:language]][tree] << @fileStore.name(fileInfo)
            end
        end
        commands
    end

    protected
    #Pure virtual functions
    def getSettings_(file, tree)
        #Should return a hash with entries
        # * :compilerSettings: an array of compiler settings
        # * :referencedFiles: an array of filepaths of referenced files (recursive)
        # * :includeDirs: an array of required include directories (internal)
        # * :language: a symbol indicating the language
        raise("Please implement this pure virtual method")
    end

    private
    @@objectExtensionPerOS = {"Linux" => ".o", "Windows" => ".obj", "MinGW" => ".obj"}
    def createFileInfoForObjectFile_(file, tree)
        fileInfo = nil

        getSettings_(file, tree).tap do |settings|
            if settings
                # Create the FileInfo object
                fileInfo =  FileInfo.new(File.basename(file.filename) + @@objectExtensionPerOS[operatingSystem])
                #Mandatory options
                fileInfo[:sourceFile] = file.filepath
                fileInfo[:language] = settings[:language]
                fileInfo[:settings] = settings[:compilerSettings].join(" ")
                fileInfo[:includeDirs] = settings[:includeDirs]
                #Optional options for improved dependency
                fileInfo[:date] = File.new(file.filepath).mtime.to_s
                fileInfo[:dates] = settings[:referencedFiles].collect{|fn|File.new(fn).mtime.to_s}.sort
                fileInfo[:directory] = file.directory
            end
        end

        fileInfo
    end
end

class DCompileFactory < CompileFactory
    def initialize
        super(:d)
    end
    def getSettings_(file, tree)
        settings = nil

        if ".d" == file.extension
            (settings ||= {})[:language] = :d
            findInfoPerModule(file.filepath, tree).tap do |infoPerModule|
                #:compilerSettings
                [].tap do |compilerSettings|
                    infoPerModule.each do |mod, info|
                        if info
                            filepath = info.filepath
                            tree.root.language[:d][:compiler].each do |when_, setting|
                                case when_
                                when :always
                                    compilerSettings << setting
                                when Regex, String
                                    compilerSettings << setting if filepath[when_]
                                end
                            end
                        end
                    end
                    compilerSettings << "-version=UnitTest" if tree.anchorPoint == file.filepath
                    settings[:compilerSettings] = compilerSettings.compact.uniq.sort
                end
            end.tap do |infoPerModule|
                #:referencedFiles
                settings[:referencedFiles] = infoPerModule.values.compact.collect{|info|info.filepath}
            end.tap do |infoPerModule|
                #:includeDirs
                includeDirs = {}
                infoPerModule.each do |mod, info|
                    if info
                        filepath = info.filepath
                        includeDirs[filepath[0, filepath.index(mod.gsub(".", "/"))]] = true
                    end
                end
                settings[:includeDirs] = includeDirs.keys.sort
            end
        end

        settings
    end
end

class DLanguage
    def initialize
        @dependency = DDependency.new
    end
    def modulesPerFile(filepath)
        @dependency.importedModules(filepath)
    end
    def moduleInfo(mod, tree)
        info = nil
        modAsFile = mod.gsub(".", "/")
        tree.each do |file|
            if file.filepath[modAsFile]
                info = file
                break
            end
        end
        info
    end
end

class CPPCompileFactory < CompileFactory
    def initialize
        super(:cpp)
    end
end
