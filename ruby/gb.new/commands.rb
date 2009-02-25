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
  def initialize(tree, fileName)
    @tree, @fileName = tree, fileName
  end
  def execute
    includedFiles = Dependency.includedFiles(@fileName)
    puts("#{@fileName}: #{includedFiles}")
  end
end

if __FILE__ == $0
  GitCommand.new(:tree, :command)
end
