require("tools/parser")

class Tree
  attr :rootTree, true
  attr :base, true
  attr :file, true
  def initialize(base, file)
    @base, @file = base, file
    @rootTree = Tree.create(@base, %w[root.tree])
  end

  def baseFile
    File.expand_path(@file, @base)
  end

  def Tree.create(pwd, wantedFiles = nil)
    wantedFiles = %w[root.tree main.cpp main.d main.rb test.cpp test.d test.rb] if wantedFiles.nil?
    res = nil
    base, file = Tree.findBaseFile(pwd, wantedFiles)
    case file
    when "root.tree"
      res = RootTree.new(base, file)
    else
      res = Tree.new(base, file)
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
end

class RootTree < Tree
  def initialize(base, file)
    @base, @file = base, file
    @rootTree = self
    parser = Parser.new(RootTreeParser)
    parser.parse(baseFile, self)
  end
  def pushURL
    raise "Not implemented"
  end
  def pullURL
    raise "Not implemented"
  end
end

module RootTreeParser
  def prepareParsing
  end

  def complete4Parser(obj)
    case obj

    when RootTree
      rt = obj
      matches(/\A[ \n]*push\.([a-zA-Z]*)/) do |hostName|
        puts("hostName = #{hostName}")
      end
      puts("Parsing RootTree")

    end
    nil
  end

  def finishedParsing
  end
end

if __FILE__ == $0
  tree = Tree.new(Dir.pwd)
end
