require("digest/md5")
require("fileutils")

require("patterns/command")
require("gubg/utils")

require("gb/dependency")

class GitCommand < ICommand
    def initialize(tree, command)
        @tree, @command = tree, command
    end
    def execute
        cmd = nil
        case @command
        when "push"
            cmd = "git push #{@tree.pushURL}"
        when "pull"
            cmd = "git pull #{@tree.pullURL}"
        when "s"
            puts("Local number of lines = #{localNrLines}")
            globalCount = globalNrLines do |base, count|
                puts("\tSubtree #{base} = #{count}")
            end
            puts("Global number of lines = #{globalCount}")
            cmd = "git status"
        when "c"
            cmd = "git commit -a"
        else
            raise "Unknown git command \"#{@command}\""
        end
        puts("Executing git command \"#{cmd}\"")
        Dir.chdir(@tree.base) do
            system(cmd)
        end
    end
    def localNrLines
        numberOfLines(@tree)
    end
    def globalNrLines
        nr = 0
        Tree.allTrees(@tree.base).each do |tree|
            count = numberOfLines(tree)
            yield(tree.base, count)
            nr += count
        end
        nr
    end
    def numberOfLines(tree)
        nr = 0
        tree.each(false) do |dir, fn|
            nr += Array.load(dir + '/' + fn).length
        end
        nr
    end
end

class CompileCommand
    #fileInfo is object file to be compiled, following options should be provided:
    # * :sourceFile
    # * :language: :d or :cpp
    # * :settings: Compiler settings to be used
    # * :includeDirs
    def initialize(fileInfo, fileStore)
        @fileInfo, @fileStore = fileInfo, fileStore
    end
    def execute
        wasCreated = @fileStore.create(@fileInfo) do |fileName|
            cmd = nil
            case @fileInfo[:language]
            when :cpp
                cmd = "g++ -c #{@fileInfo[:sourceFile]} #{@fileInfo[:settings]} -o #{fileName}"
                @fileInfo[:includeDirs].each { |id| cmd += " -I#{id}"}
            when :d
                cmd = "dmd -c #{@fileInfo[:sourceFile]} #{@fileInfo[:settings]} -of#{fileName}"
                @fileInfo[:includeDirs].each { |id| cmd += " -I#{id}"}
            else
                raise "Unknown language \"#{@fileInfo[:language]}\""
            end
            puts(cmd)
            system(cmd)
        end
        #    puts("\"#{@fileStore.name(@fileInfo)}\" => Found in cache.") if !wasCreated
    end
    def output
        @fileStore.name(@fileInfo)
    end
end

class LinkCommand
    #fileInfo should contain:
    # * :execName
    # * :objectFiles
    # * :language: :d or :cpp
    # * :settings: Linker settings to be used
    def initialize(fileInfo, fileStore)
        @fileInfo, @fileStore = fileInfo, fileStore
    end
    def execute
        wasCreated = @fileStore.create(@fileInfo) do |fileName|
            objectFiles = @fileInfo[:objectFiles]
            dir = "./"
            if (Collection.new("MinGW", "Windows") === operatingSystem)
                fileName = fileName.gsub("/", "\\\\\\\\")
                objectFiles = objectFiles.collect do |objectFile|
                    objectFile.gsub(/^(.+)\//, "")
                end
                dir = Dir.pwd[0,2] + "/tmp/gb"
            end
            cmd = nil
            case @fileInfo[:language]
            when :cpp
                cmd = "g++ #{objectFiles.join(' ')} #{@fileInfo[:settings]} -o #{fileName}"
            when :d
                cmd = "dmd #{objectFiles.join(' ')} #{@fileInfo[:settings]} -of#{fileName}"
            end
            puts(cmd)
            Dir.chdir(dir) do
                puts(Dir.pwd)
                system(cmd)
            end
        end
        # Copy the file from the file store to its proper location
        FileUtils.copy(@fileStore.name(@fileInfo), @fileInfo[:execName])
        FileUtils.chmod(0755, @fileInfo[:execName])
    end
end

class ArchiveCommand
    #fileInfo should contain
    # * :objects: The object files to link into the lib
    # * :libName: Obvious
    def initialize(fileInfo, fileStore)
        @fileInfo, @fileStore = fileInfo, fileStore
    end
    def execute
        wasCreated = @fileStore.create(@fileInfo) do |fileName|
            cmd = nil
            case operatingSystem
            when "Linux"
                cmd = "ar rcs #{fileName} " + @fileInfo[:objects].join(" ")
            when Collection.new("MinGW", "Windows")
                cmd = "lib -c -p32 #{fileName} " + @fileInfo[:objects].join(" ")
            end
            puts(cmd)
            system(cmd)
        end
        # Copy the file from the file store to its proper location
        File.dirname(@fileInfo[:libName]).tap{|dir|FileUtils.mkdir_p(dir) if !File.exist?(dir)}
        FileUtils.copy(@fileStore.name(@fileInfo), @fileInfo[:libName])
    end
end

class ExecuteCommand
    def initialize(cmd, dir = nil)
        @cmd, @dir = cmd, (dir || Dir.pwd )
    end
    def execute
        Dir.chdir(@dir) do
            puts("WARNING::\"#{@cmd}\" in \"#{@dir}\" failed.") if !system(@cmd)
        end
    end
end

class FormatCommand
    def initialize(fileName, style)
        @fileName, @style = fileName, style
    end
    def execute
        cmd = "astyle --style=#{@style} #{@fileName}"
        puts("Formatting \"#{@fileName}\" using style \"#{@style}\".")
        output = `#{cmd}`
        if $? != 0
            raise "Could not format \"#{@fileName}\" with style \"#{@style}\""
        else
            FileUtils.rm(@fileName + ".orig")
        end
    end
end

class CopyCommand
    def initialize(from, to)
        @from, @to = from, to
    end
    def execute
        FileUtils.copy(@from, @to)
    end
end

if __FILE__ == $0
    GitCommand.new(:tree, :command)
end
