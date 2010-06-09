require("commands")
require("dependency")
require("gubg/utils")
require("gubg/filestore")

def selectCommandFactory(tree, operation)
    res = nil
    case operation
    when :build
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
            res = DCommandFactory.new
        when ".cpp"
            raise("I cannot combine cpp and d yet") if countPerExtension[".d"] > 0
            res = CPPCommandFactory.new
        else
            raise("Could not find either d or cpp source files")
        end
    else
        raise("Unknown operation #{operation}")
    end
    res
end

class CommandFactory
    def initialize
        @fileStore = FileStore.new("/tmp/gb")
    end
    def createCommands(tree)
        commands = []
        tree.each do |file|
            createFileInfoForObjectFile_(file, tree).tap do |fileInfo|
                commands << CompileCommand.new(fileInfo, @fileStore) if fileInfo
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
        # * :language: a string indicating the language
        raise("Please implement this pure virtual method")
    end
    def modulesPerFile_(filepath)
        #Should return an array of modules referenced in filepath (not recursive)
        raise("Please implement this pure virtual method")
    end
    def moduleInfo_(mod, tree)
        #Should return information on mod if mod is found internally
        #Else, nil
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
                fileInfo["settings"] = settings[:compilerSettings].join(" ")
                fileInfo["includeDirs"] = settings[:includeDirs]
                fileInfo["date"] = File.new(file.filepath).mtime.to_s
                fileInfo["dates"] = settings[:referencedFiles].collect{|fn|File.new(fn).mtime.to_s}.sort
                fileInfo["sourceFile"] = file.filepath
                fileInfo["directory"] = file.directory
                fileInfo["language"] = settings[:language]
            end
        end

        fileInfo
    end
    def findInfoPerModule_(filepath, tree)
        puts("Finding module info for #{filepath}") if $verbose
        infoPerModule = {}
        modules2Check = modulesPerFile_(filepath)
        while !modules2Check.empty?
            newModules2Check = []
            modules2Check.uniq.each do |mod|
                if info = moduleInfo_(mod, tree)
                    modulesPerFile_(info.filepath).each do |newMod|
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

class DCommandFactory < CommandFactory
    def initialize()
        @dependency = DDependency.new
        super
    end

    protected
    def getSettings_(file, tree)
        settings = nil

        if ".d" == file.extension
            (settings ||= {})[:language] = "d"
            findInfoPerModule_(file.filepath, tree).tap do |infoPerModule|
                #:compilerSettings
                [tree.root.language[:d][:compiler][:always]].tap do |compilerSettings|
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
    def modulesPerFile_(filepath)
        @dependency.importedModules(filepath)
    end
    def moduleInfo_(mod, tree)
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

class CPPCommandFactory < CommandFactory
    def initialize()
        @dependency = CPPDependency.new
        super
    end
end
