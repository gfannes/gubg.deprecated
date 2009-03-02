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
      cmd = "git push #{@tree.pullURL}"
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
  def initialize(base, source, includeDirs, settings)
    @base, @source, @includeDirs, @settings = base, source, includeDirs, settings
  end
  def execute
    FileUtils.mkdir_p(baseDir) if !File.exist?(baseDir)
    command = "gcc -c -o #{output} #{@source} #{@settings}"
    @includeDirs.each do |id|
      command += " -I#{id}"
    end
    puts(command)
    raise "Compilation failed." if !system(command)
  end
  def baseDir
    File.expand_path(".obj/" + Digest::MD5.hexdigest(@settings), @base)
  end
  def output
    File.expand_path(File.basename(@source, ".cpp") + ".o", baseDir)
  end
end

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
