require("digest/md5")
require("fileutils")

require("patterns/command")

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
end

class CompileCommand
  def initialize(fileInfo, fileStore)
    @fileInfo, @fileStore = fileInfo, fileStore
  end
  def execute
    wasCreated = @fileStore.create(@fileInfo) do |fileName|
      cmd = nil
      case @fileInfo["type"]
      when "cpp"
        cmd = "g++ -c #{@fileInfo['sourceFile']} #{@fileInfo['settings']} -o #{fileName}"
        @fileInfo["includeDirs"].each { |id| cmd += " -I#{id}"}
      when "d"
        cmd = "dmd -c #{@fileInfo['sourceFile']} #{@fileInfo['settings']} -of#{fileName}"
        @fileInfo["includeDirs"].each { |id| cmd += " -I#{id}"}
      else
        raise "Unknown type \"#{@fileInfo["type"]}\""
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
  def initialize(fileInfo, fileStore)
    @fileInfo, @fileStore = fileInfo, fileStore
  end
  def execute
    wasCreated = @fileStore.create(@fileInfo) do |fileName|
      cmd = "#{@fileInfo['linker']} #{@fileInfo['objectFiles'].join(' ')} #{@fileInfo['settings']} -o #{fileName}"
      puts(cmd)
      system(cmd)
    end
    # Copy the file from the file store to its proper location
    FileUtils.copy(@fileStore.name(@fileInfo), @fileInfo["execName"])
    FileUtils.chmod(0755, @fileInfo["execName"])
  end
end

if __FILE__ == $0
  GitCommand.new(:tree, :command)
end
