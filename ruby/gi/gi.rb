require("gi/environment")
require("gubg/utils")

class GenericInstaller
  def initialize
    @commands = []
    @env = Environment.new
  end

  def parseArguments(arguments)
    arguments.each do |package|
      addInstallCommand(package)
    end
  end

  def addInstallCommand(package)
    require("gi/packages/#{package}")
    installer = nil
    eval("installer = Install_#{package}.new(@env)")
    @commands << installer
  end

  def executeCommands
    time("Executing all commands (#{@commands.length})", true) do
      @commands.each do |command|
        command.execute
      end
    end
  end

  def print(what, args = nil)
    case what
    when :usage
      puts(%Q@Usage:: gi <package> <package> ...
Created by Geert Fannes under GPL.
@)
    when :unknownCommand
      puts("ERROR::Unknown command \"#{args}\"")
    else
    end
  end

  def indent(level)
    " "*3*level
  end
end

if __FILE__ == $0
end