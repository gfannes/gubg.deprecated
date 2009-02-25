require("yaml")

require("patterns/chainOfResponsibility")
require("tools/utils")

class Tree# < IChainOfResponsibility
  attr :rootTree, true
  attr :base, true
  attr :file, true
  attr :successor, true
  @@definingFiles = %w[root.tree disabled.tree main.cpp main.d test.cpp test.d test.rb]
  @@cppFile = /\.cpp$/
  @@hppFile = /\.hpp$/
  @@dFile = /\.d$/
  @@wantedFiles = [@@cppFile, @@hppFile, @dFile]
  def initialize(base, file)
    @base, @file = base, file
    loadSettings
  end

  def buildCommands(command)
    commands = []
    case command
    when NilClass
      case @file
      when "root.tree"
        # Build all
        each do |dir, fn|
          case fn
          when @@cppFile
            commands << CompileCommand.new(self, File.expand_path(fn, dir))
          when @@dFile
          end
        end
      when "test.cpp"
        # Build system test
        each do |dir, fn|
          puts("#{dir} #{fn}")
        end
      else
        raise "Not supported"
      end
    else
      raise "Not supported"
    end
    commands
  end

  def each
    recursor = Proc.new do |dir|
      res = true
      @@definingFiles.each do |fn|
        res = false if File.exist?(File.expand_path(fn, dir))
      end
      res
    end
    Dir.each(@base, recursor) do |dir, fn|
      case fn
      when Collection.from(@@wantedFiles)
        yield(dir, fn)
      end
      nil
    end
    # Iterate also over the files of the successor
    @successor.each{|dir, fn|yield(dir, fn)} if !@successor.nil?
  end

  def baseFile
    File.expand_path(@file, @base)
  end

  def Tree.create(pwd, definingFiles = nil)
    definingFiles = @@definingFiles if definingFiles.nil?
    res = nil
    prevTree = nil
    base = pwd
    while true
      base, file = Tree.findBaseFile(base, definingFiles)
      if res.nil?
        prevTree = res = Tree.new(base, file)
      else
        prevTree.successor = Tree.new(base, file)
        prevTree = prevTree.successor
      end
      case file
      when "root.tree"
        break
      when "disabled.tree"
        raise "You are inside a disabled tree"
      else
      end
      base = File.dirname(base)
    end
    res
  end

  def Tree.findBaseFile(pwd, files)
    base, file = nil, nil
    # Recursively descend until one of these files has been found
    here = pwd
    while base.nil?
      # Try to find a match
      files.each do |lfile|
        if File.exist?(File.expand_path(lfile, here))
          base = here
          file = lfile
          puts("I found basefile \"#{file}\" at \"#{base}\"")
          break
        end
      end
      break if !base.nil?
      # No match was found, decrease "here" if possible
      tmp = File.dirname(here)
      raise "Could not find any basefile from \"#{pwd}\" (possibilities: #{files})" if tmp == here
      here = tmp
    end
    return base, file
  end

  def pushURL
    if !@settings.nil?
      @settings["git"]["push"][ENV["USER"]]
    else
      @successor.pushURL
    end
  end
  def pullURL
    if !@settings.nil?
      @settings["git"]["pull"][ENV["USER"]]
    else
      @successor.pullURL
    end
  end
  def loadSettings
    if @file == "root.tree"
      @settings = YAML::load(File.open(File.expand_path(@file, @base)))
    else
      @settings = nil
    end
  end
end

if __FILE__ == $0
  tree = Tree.new(Dir.pwd)
end
