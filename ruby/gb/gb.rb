require("tools/utils")

require("gb/commands")
require("gb/tree")

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
        when Collection.from(%w[push pull s c])
          # Git commands
          setCommand(:git, argument)
        when "all"
          # Build all
          setCommand(:build, argument)
        when "clean"
          # Clean all
          setCommand(:clean, argument)
        when Collection.from(%w[f format])
          # Format using astyle
          setCommand(:format, argument)
        else
          location = File.expand_path(argument)
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
      @commands << GitCommand.new(Tree.create(location), @command)
    when :build
      trees = nil
      case @command
      when NilClass
        trees = [Tree.create(location)]
      when "all"
        trees = Tree.allTrees(location)
        @command = nil
      end
      puts("I will build the following trees:")
      trees.each do |tree|
        puts(" * #{tree.base} (#{tree.file})")
      end
      trees.each do |tree|
        @commands += tree.buildCommands(@command)
      end
      # Execute the compiled unit tests
      trees.each do |tree|
        if tree.unitTest?
          @commands << ExecuteCommand.new(tree.execName, File.dirname(tree.execName))
        end
      end
    when :clean
      Tree.cleanFileStore
    when :format
      trees = Tree.allTrees(location)
      puts("I will format the following trees:")
      trees.each do |tree|
        puts(" * #{tree.base} (#{tree.file})")
      end
      trees.each do |tree|
        @commands += tree.formatCommands
      end
    when :unknownCommand
      print(:unknownCommand, @command)
    else
      print(:unknownCommand)
    end
  end

  def executeCommands
    time("Executing all commands", true) do
      @commands.each do |command|
        command.execute
      end
    end
  end

  def print(what, args = nil)
    case what
    when :usage
      puts(%Q@Usage:: gb [command] [location]
#{indent(1)}Git commands:
#{indent(2)} * push: push to the location specified in root.tree
#{indent(2)} * pull: pull from the location specified in root.tree
#{indent(2)} * s: status
#{indent(2)} * c: commit -a
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
