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
    @fileStore.create(@fileInfo) do |fileName|
      cmd = nil
      case @fileInfo["type"]
      when "cpp"
        cmd = "gcc -c -o #{fileName} #{@fileInfo['sourceFile']} #{@fileInfo['settings']}"
        @fileInfo["includeDirs"].each { |id| cmd += " -I#{id}"}
      when "d"
      else
        raise "Unknown type \"#{@fileInfo["type"]}\""
      end
      puts("Executing \"#{cmd}\"")
      system(cmd)
    end
  end
end

# class CPPCompileCommand < CompileCommand
#   def command
#     cmd = "gcc -c -o #{output} #{@source} #{@settings}"
#     @includeDirs.each do |id|
#       cmd += " -I#{id}"
#     end
#     cmd
#   end
#   def extension
#     ".cpp"
#   end
# end

# class DCompileCommand
#   def command
#     cmd = "dmd -c -of#{output} #{@source} #{@settings}"
#     @includeDirs.each do |id|
#       cmd += " -I#{id}"
#     end
#     cmd
#   end
#   def extension
#     ".d"
#   end
# end

class LinkCommand
  def initialize(exec, objects, settings)
    @exec, @objects, @settings = exec, objects, settings
  end
  def execute
    command = "gcc -o #{@exec} #{@settings} " + @objects.join(" ")
    puts(command)
    raise "Linking failed." if !system(command)
  end
end

if __FILE__ == $0
  GitCommand.new(:tree, :command)
end
