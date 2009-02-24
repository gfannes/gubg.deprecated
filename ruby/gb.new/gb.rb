require("tools/utils")

require("commands")

class GenericBuild
  def initialize
    @commands = []
  end

  def parseArguments(arguments)
    # Check the arguments and extract the command and location
    setCommand(:reset)
    location = nil
    begin
      arguments.each do |argument|
        case argument
        when Collection.from(%w[--help -h --])
          # Help commands
          setCommand(:help)
          break
        when Collection.from(%w[add commit push pull])
          # Git commands
          setCommand(:git, argument)
        else
          location = File.expand_path(argument)
          location.freeze
        end
      end
    rescue
      setCommand(:unknownCommand, arguments)
    end

    # By default, we build
    setCommand(:build) if @commandType.nil?
    # By default, we take the current working directory as location
    location = Dir.pwd if location.nil?

    # Create the command objects depending on the command
    case @commandType
    when :help
      print(:usage)
    when :git
    when :build
    when :unknownCommand
      print(:unknownCommand, @command)
    end
  end

  def executeCommands
    @commands.each do |command|
      command.execute
    end
  end

  def print(what, args = nil)
    case what
    when :usage
      puts(%Q@Usage:: gb [command] [location]
#{indent(1)}Git commands:
#{indent(2)} * add
#{indent(2)} * commit
#{indent(2)} * push
#{indent(2)} * pull
#{indent(1)}Location: The tree to be used. If not specified, the working directory is taken.
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

  def setCommand(type, command = nil)
    case type
    when :reset
      @commandType = nil
      @command = nil
    else
      raise "ERROR::command is already set" if !@command.nil?
      @commandType = type
      @command = command
    end
  end
end

if __FILE__ == $0
  gb = GenericBuild.new
  gb.parseArguments(ARGV.dup)
  gb.executeCommands
end
