require("patterns/command")

require("gb.new/dependency")

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
  def initialize(source, includeDirs, settings)
    @source, @includeDirs, @settings = source, includeDirs, settings
  end
  def execute
    command = "gcc -c -o #{output} #{@source} #{@settings}"
    @includeDirs.each do |id|
      command += " -I#{id}"
    end
    puts(command)
    raise "Compilation failed." if !system(command)
  end
  def output
    @source.gsub(/\.cpp$/, ".o")
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
